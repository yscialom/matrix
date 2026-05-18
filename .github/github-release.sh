#!/usr/bin/env bash
# release.sh — processus de release semi-automatisé pour ysc::matrix
#
# Usage :
#   ./release.sh prepare  M.m.p   — branche release, bump CMakeLists.txt, PR vers master
#   ./release.sh finalize M.m.p   — CHANGELOG + tag signé sur la branche release

set -euo pipefail

die()         { echo "ERREUR: $*" >&2; exit 1; }
require_cmd() { command -v "$1" &>/dev/null || die "commande manquante : $1"; }
separator()   { echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"; }

# ---------------------------------------------------------------------------
# ensure_git_cliff — utilise git-cliff système ou le télécharge dans /tmp
# ---------------------------------------------------------------------------
GIT_CLIFF_VERSION="2.13.1"
ensure_git_cliff() {
    if command -v git-cliff &>/dev/null; then return 0; fi
    local bin_dir="/tmp/git-cliff-bin"
    local bin="${bin_dir}/git-cliff"
    if [[ -x "$bin" ]]; then export PATH="${bin_dir}:${PATH}"; return 0; fi
    echo "==> git-cliff non trouvé — téléchargement de la version ${GIT_CLIFF_VERSION}..."
    mkdir -p "$bin_dir"
    curl -sSfL \
        "https://github.com/orhun/git-cliff/releases/download/v${GIT_CLIFF_VERSION}/git-cliff-${GIT_CLIFF_VERSION}-x86_64-unknown-linux-musl.tar.gz" \
        | tar -xz --strip-components=1 -C "$bin_dir" "git-cliff-${GIT_CLIFF_VERSION}/git-cliff"
    export PATH="${bin_dir}:${PATH}"
}

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
    dashboard=$(sed -n '/^## Vue par épopée/,/^## EPIC/{/^## EPIC/d; /^## Vue par/d; p}' doc/user-stories.md)

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
    echo "  1. Attendre que la CI soit verte sur la PR"
    echo "  2. Lancer : ./.github/github-release.sh finalize ${version}"
    separator
}

# ---------------------------------------------------------------------------
# phase_finalize VERSION
# ---------------------------------------------------------------------------
phase_finalize() {
    local version="$1"
    parse_version "$version"
    local tag="v${version}"
    local branch="release/${tag}"

    require_cmd git
    require_cmd gh
    ensure_git_cliff
    check_clean_worktree

    local current_branch
    current_branch=$(git symbolic-ref --short HEAD 2>/dev/null) \
        || die "HEAD détaché — positionnez-vous sur ${branch}"
    [[ "$current_branch" == "$branch" ]] \
        || die "branche courante '${current_branch}' ≠ '${branch}'"

    local actual
    actual=$(cmake_version)
    [[ "$actual" == "$version" ]] \
        || die "CMakeLists.txt affiche ${actual} et non ${version}"

    git pull --ff-only origin "$branch"

    echo "==> Génération du CHANGELOG..."
    git-cliff --config cliff.toml --tag "$tag" --output CHANGELOG.md
    git add CHANGELOG.md
    if ! git diff --cached --quiet; then
        git commit -m "chore(release): update CHANGELOG for ${tag}"
        git push origin "$branch"
    fi

    echo "==> Création du tag signé ${tag} sur ${branch}..."
    git tag -s -a "$tag" -m "Release ${tag}"
    git push origin "$tag"

    separator
    echo "  Tag ${tag} poussé sur ${branch}."
    echo "  La CI crée le GitHub Release automatiquement."
    echo ""
    echo "  Prochaines étapes (manuelles) :"
    echo "  1. Attendre que la CI (déclenchée par le tag) soit verte"
    echo "  2. Merger la PR ${branch} → master (merge commit)"
    echo "  3. Créer et merger la PR back-merge master → develop :"
    echo "       gh pr create --base develop --head master \\"
    echo "         --title 'chore: back-merge ${tag} into develop' \\"
    echo "         --body 'Back-merge after release ${tag}.'"
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
