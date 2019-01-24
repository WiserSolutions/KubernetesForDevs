#!/bin/bash
# github-creds <namespace> <release-name>
#
# Generate ssh credentials that can be used as a github deployment key to pair with
# the gitsync keys used by airflow
#
# Args:
#  - <namespace> <release-name> : is the helm release name that you used to deploy airflow
#
namespace=$1
namespace=${namespace:-default}
secret=$2
secret=${secret:-gitcreds}

echo "Generating the  sshkey"
ssh-keygen -t rsa -b 4096  -N '' -f ${secret}_rsa

echo "Generating known_hosts for github"
ssh-keyscan github.com > ${secret}_known_hosts

echo "Generating secret file"
kubectl -n ${namespace} create secret generic ${secret} --from-file=ssh=${secret}_rsa --from-file=known_hosts=${secret}_known_hosts -o yaml --dry-run >${secret}.yaml

echo "Copy and paste the follwing (from airflow_rsa.pub) to the deploy keys for your github repo"
cat ${secret}_rsa.pub


