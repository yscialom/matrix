#!/usr/bin/env bash
# release.sh — processus de release semi-automatisé pour ysc::matrix
#
# Usage :
#   ./release.sh prepare  M.m.p   — branche release, bump CMakeLists.txt, PR vers master
#   ./release.sh finalize M.m.p   — tag signé sur master, PR back-merge vers develop

set -euo pipefail

die()         { echo "ERREUR: $*" >&2; exit 1; }
require_cmd() { command -v "$1" &>/dev/null || die "commande manquante : $1"; }
separator()   { echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"; }

# ---------------------------------------------------------------------------
# parse_version VERSION → sets MAJOR MINOR PATCH
# ---------------------------------------------------------------------------
parse_version() {
    local v="$1"
    [[ "$v" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] \
        || die "format de version invalide : '$v' (attendu M.m.p)"
    MAJOR=$(cut -d. -f1 <<<"$v")
    MINOR=$(cut -d. -f2 <<<"$v")
    PATCH=$(cut -d. -f3 <<<"$v")
}

# ---------------------------------------------------------------------------
# check_clean_worktree — échoue si des modifications non commitées existent
# ---------------------------------------------------------------------------
check_clean_worktree() {
    git diff --quiet && git diff --cached --quiet \
        || die "modifications non commitées détectées — stashez ou commitez d'abord"
}

# ---------------------------------------------------------------------------
# cmake_version → affiche la version lue dans CMakeLists.txt
# ---------------------------------------------------------------------------
cmake_version() {
    local maj min pat
    maj=$(grep -oP 'set\(VERSION_MAJOR\s+\K\d+' CMakeLists.txt)
    min=$(grep -oP 'set\(VERSION_MINOR\s+\K\d+' CMakeLists.txt)
    pat=$(grep -oP 'set\(VERSION_PATCH\s+\K\d+' CMakeLists.txt)
    echo "${maj}.${min}.${pat}"
}

# ---------------------------------------------------------------------------
# phase_prepare VERSION
# ---------------------------------------------------------------------------
phase_prepare() {
    local version="$1"
    parse_version "$version"
    local tag="v${version}"
    local branch="release/${tag}"

    require_cmd git
    require_cmd gh
    check_clean_worktree

    echo "==> Création de la branche ${branch}..."
    git checkout -b "$branch"

    echo "==> Mise à jour de CMakeLists.txt (${version})..."
    sed -i -E "s/set\(VERSION_MAJOR[[:space:]]+[0-9]+/set(VERSION_MAJOR   ${MAJOR}/" CMakeLists.txt
    sed -i -E "s/set\(VERSION_MINOR[[:space:]]+[0-9]+/set(VERSION_MINOR   ${MINOR}/" CMakeLists.txt
    sed -i -E "s/set\(VERSION_PATCH[[:space:]]+[0-9]+/set(VERSION_PATCH   ${PATCH}/" CMakeLists.txt

    local actual
    actual=$(cmake_version)
    [[ "$actual" == "$version" ]] \
        || die "le bump a échoué : CMakeLists.txt affiche ${actual} au lieu de ${version}"

    echo "==> Extraction du tableau de bord des épopées..."
    local dashboard
    dashboard=$(sed -n '/^## Vue par épopée/,/^## EPIC/{/^## EPIC/d; /^## Vue par/d; p}' user-stories.md)

    echo "==> Commit..."
    git add CMakeLists.txt
    git commit -m "Release ${tag}" -m "${dashboard}"

    echo "==> Push..."
    git push -u origin "$branch"

    echo "==> Ouverture de la PR vers master..."
    local pr_url
    pr_url=$(gh pr create \
        --base master \
        --title "Release ${tag}" \
        --body "$(printf 'Release %s\n\n%s' "$tag" "$dashboard")")

    separator
    echo "  PR créée : ${pr_url}"
    echo ""
    echo "  Prochaines étapes (manuelles) :"
    echo "  1. Attendre que la CI soit verte"
    echo "  2. Merger la PR (merge commit) dans master"
    echo "  3. Lancer : ./release.sh finalize ${version}"
    separator
}

# ---------------------------------------------------------------------------
# phase_finalize VERSION
# ---------------------------------------------------------------------------
phase_finalize() {
    local version="$1"
    parse_version "$version"
    local tag="v${version}"

    require_cmd git
    require_cmd gh
    check_clean_worktree

    echo "==> Synchronisation de master..."
    git fetch origin
    git checkout master
    git merge --ff-only origin/master

    local actual
    actual=$(cmake_version)
    [[ "$actual" == "$version" ]] \
        || die "master affiche la version ${actual} et non ${version} — la PR release est-elle mergée ?"

    echo "==> Création du tag signé ${tag}..."
    git tag -s -a "$tag" -m "Release ${tag}"
    git push origin "$tag"

    echo "==> Ouverture de la PR back-merge master → develop..."
    local pr_url
    pr_url=$(gh pr create \
        --base develop \
        --head master \
        --title "chore: back-merge ${tag} into develop" \
        --body "Back-merge after release ${tag} — récupère le tag dans l'historique de develop.")

    separator
    echo "  Tag ${tag} poussé."
    echo "  PR back-merge créée : ${pr_url}"
    echo ""
    echo "  Dernière étape (manuelle) :"
    echo "  1. Merger la PR back-merge dans develop"
    separator
}

# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------
main() {
    local phase="${1:-}" version="${2:-}"

    if [[ -z "$phase" || -z "$version" ]]; then
        echo "Usage: $0 prepare|finalize M.m.p" >&2
        exit 1
    fi

    case "$phase" in
        prepare)  phase_prepare  "$version" ;;
        finalize) phase_finalize "$version" ;;
        *) die "phase inconnue : '$phase' (prepare|finalize)" ;;
    esac
}

main "$@"
