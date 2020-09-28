#!/bin/sh
# set -o errexit
# set -o xtrace
set -o nounset
CONTR_BASENAME=drcrane/falcon-website
TAG_LIST_TMP=$(mktemp /tmp/falcon-deploy-tag-list.XXXXXXXX)
VERSION_TAG="${1}"
EXIT_STATUS=0
./dockertags.sh "${CONTR_BASENAME}" |tee "${TAG_LIST_TMP}" |grep "${VERSION_TAG}"
if [ "$?" -eq 0 ] ; then
minikube kubectl -- set image deployment/falcon-website-deployment "falcon-website-container=${CONTR_BASENAME}:${VERSION_TAG}"
else
EXIT_STATUS=1
echo "version tag not found, these were found:"
cat "${TAG_LIST_TMP}"
fi
rm "${TAG_LIST_TMP}"
exit ${EXIT_STATUS}

