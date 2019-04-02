#!/bin/bash

git checkout gh-pages
git rm -fr "${GITHUB_PAGES_DIR}"
mkdir -p "${GITHUB_PAGES_DIR}"
cp -r html/* "${GITHUB_PAGES_DIR}"/.
git add "${GITHUB_PAGES_DIR}"
git commit -m"Update documentation."
git push origin gh-pages
git checkout HEAD@{1}
