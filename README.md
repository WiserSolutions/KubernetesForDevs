# Kubernetes For Devs

This workshop series hopes to be a useful primer for developers looking to bootstrap their way to using Kubernetes. workshops will be added as they become available.

1. [Workshop 1](workshop-1.md) - Getting around in Kubernetes
1. [Workshop 2](workshop-2.md) - I Can Deploy My Workloads
1. [Workshop 3](workshop-3.md) - Charting Deployments with Helm
2. [Workshop 4](workshop-4.md) - Production-ready Deployments

## Getting Started

This workshop series will attempt to have a general focus with most exercises assuming a local workstation with an assumption that this is a OS X based workstation.

While you can use [minikube](https://github.com/kubernetes/minikube), you will find it much easier to address networking and volume issues if you use the Kubernetes available through *Docker CE*. If you haven't already installed Docker with Kubernetes, download [Docker CE](https://docs.docker.com/install#desktop) now.

**Start Up Kubernetes**

With Docker CE installed, your next step is to enable Kubernetes. Start by launching the Preferences menu. You'll find this on your workstation's menu bar.

With the Preferences menu open visit the [Kubernetes setup instructions](https://docs.docker.com/docker-for-mac#kubernetes) and follow the detailed instructions there.

**Install Helm**

Helm is [the package manager for Kubernetes](https://docs.helm.sh/) which we'll go into in more detail starting in workshop 3. 

To get the latest stable release...

```bash
$ curl https://raw.githubusercontent.com/helm/helm/master/scripts/get > get_helm.sh
$ chmod 700 get_helm.sh
$ ./get_helm.sh
Helm v2.12.1 is available. Changing from version v2.10.0.
Downloading https://kubernetes-helm.storage.googleapis.com/helm-v2.12.1-darwin-amd64.tar.gz
Preparing to install helm and tiller into /usr/local/bin
Password:
helm installed into /usr/local/bin/helm
tiller installed into /usr/local/bin/tiller
Run 'helm init' to configure helm.
# Install it on Kubernetes
$ helm init
$HELM_HOME has been configured at /Users/<your-home>/.helm.

Tiller (the Helm server-side component) has been installed into your Kubernetes Cluster.

Please note: by default, Tiller is deployed with an insecure 'allow unauthenticated users' policy.
To prevent this, run `helm init` with the --tiller-tls-verify flag.
For more information on securing your installation see: https://docs.helm.sh/using_helm/#securing-your-helm-installation
Happy Helming!
# Test it
$ helm version
Client: &version.Version{SemVer:"v2.12.1", GitCommit:"02a47c7249b1fc6d8fd3b94e6b4babf9d818144e", GitTreeState:"clean"}
Server: &version.Version{SemVer:"v2.12.1", GitCommit:"02a47c7249b1fc6d8fd3b94e6b4babf9d818144e", GitTreeState:"clean"}
~~
```

Done! You are now ready to start the workshops

