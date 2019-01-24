
[KubernetesForDevs](/README.md) >> workshop-2

# Workshop 2 - Basic Object Development

This tutorial leverages some of the skills from [Workshop 1](/workshop-1.md) to begin exploring the basic Kubernetes objects that you will tend to work with most.

* Pods
  * ConfigMaps
  * Secrets
  * Volumes
* Services
* Deployments

As you go through these exercises, you should bookmark the [Kubernetes API Reference Docs](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.12/) to look up property details.

If you get stuck, answers can be found in the workshop-2 branch of this repo.

You'll want to cd to `workshop-2` for the following examples...

## Exercise 1 - Pod containers can do IPC

At its simplest a Pod runs a single container and may need no more than a few environment variables to configure it. Many other applications need more. Pod containers can share *volumes*, *network* and *IPC*. In this exercise you will build and use a provided Dockerfile into an image that will be used to:

1. Initialize an IPC message queue
2. Publish to the queue
3. Consume from the queue

Take a look at [ipc/ipc.c](workshop-2/ipc/ipc.c), it can be started in one of three ways.

1. `ipc` - will initialize the message q and push an initializaiton message on the queue
2. `ipc -producer` - will read the contents of stdin and push it onto the queue
3. `ipc -consumer` - will pop the contents off of the queue and print it to stdout


First build the image:

```bash
$ docker build -t ipc-example workshop-2/ipc
```

Next we'll need to compose the yaml for our pod. Open your editor and create `ipc/ipc.yaml`

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: 
spec:
  initContainers:
  
  containers:
```

At the top, like all Kubernetes objects, there is an `apiVersion` and a `kind` followed by `metadata` (with a name) and the object's `spec`.

Note that you can specify initialization containers. These are useful when resources needed by a Pod's containers need to be made available before they start. It would be a good idea in this example to have the message queue created before the other containers attached.

First we'll need to give it name, then fill out the container information. Note that containers are specified as array items; in *yaml* these will each begin with a `-`.

For this exercise, out containers will need:

* a name
* an image tag
* an imagePullPolicy of `Never` to force Kubernetes to use the image you just built on your machine
* a command - commands are an array of arguments. If you need to pipe things, you can always  use `sh -c "(cat somefile |some_command)"`

Go ahead and try to complete the pod definition and when you're ready, use kubectl to create, update and delete the pod.

```bash
$ kubectl create -f ipc
```

You can review the progress using the Dashboard, or you can review the progress via kubectl.

```bash
# Get pod status and watch for updates
$ kubectl get pods -w
NAME      READY   STATUS      RESTARTS   AGE
ws2-ipc   0/2     Completed   0          5s
^C
$ kubectl logs ws2-ipc --container consumer
Consumed: Shared Memory Queue - initialized
Consumed: The time is now Thu Jan 24 06:12:19 UTC 2019

Consumed: done
```

In this example output, the pod was named `ws2-ipc`, the producer ran `sh -c "(echo The time is now $(date) | ./ipc -producer)"` and the consumer was named `consumer`

Clean up after yourself!

```bash
$ kubectl delete -f ipc
```

## Exercise 2 - Poxies and Sidecars Oh My!

In *Exercise 1* you saw that you can build containers to represent a single process and have it cooperate with another container running in the same pod using IPC. This producer consumer pattern is fairly common in software architectures.

Other architectural patterns used with Pods include *Sidecar*, *Proxy*, *Bridge* and *Adapter*. In this exercise you'll use the *Sidecar* pattern to run a container that will sync up with a private git repo and a *Proxy* server that will act as a gateway to the back-end web service. In this example the proxy is a dumb pass-through, but in real life it could be providing TLS, circuit-breaking, filtering or other activities that are not part of the webserver that is performing your work.

Begin by stubbing out your Pod specification you'll need the following:

* metadata that includes `name` and a `label` to use later for a *Service* to connect with it; `app` is a common label.
* 3 containers, 
  * `git-sync` as a sidecar to synchronize with *github*
  * `proxy` to proxy traffic
  * `webapp` to do the heavy lifting
* 3 volumes
  * `nginx-proxy-config` of type `configMap` for the Nginx configuration that will be pulled from a configmap
  * `git-secret` of type `secret` for the git secret that contains the private key used to connect with git
  * `html` of type `emptyDir` to share the *html* data between the git sync and the webapp

**Let's start** by creating the `configMap` for the proxy server. 

The configuration for the proxy is found in [sidecar-proxy/nginx.conf](workshop-2/sidecar-proxy/nginx.conf). The proxy server will run on port `8080` and the webapp will run on port `80`.

You can create type the whole configmap in, or you can create it via a command...

```bash
$ kubectl create configmap nginx-conf --from-file sidecar-proxy/nginx.conf --dry-run -o yaml > sidecar-proxy/nginx-conf.yaml
```

**Note:** the `--dry-run` prevents the object from being sent to the cluster and the `-o yaml` specifies the output type.

Take a moment to look at the configmap

**Next**, create the secret. In this case, the credentials have been provided that will work with this example. These are `sidecar-proxy/gitcreds_rsa`, `sidecar-proxy/gitcreds_rsa.pub` and `gitcreds_known_hosts`. A handy script, `sidecar-proxy/mk-secrets.sh`, for creating these is provided if you want to experiment with your own repositories.

Creating the secret is similar to configmaps...

```bash
$ cd sidecar-proxy
$ kubectl create secret generic gitcreds --from-file=ssh=gitcreds_rsa --from-file=known_hosts=gitcreds_known_hosts -o yaml --dry-run >gitcreds.yaml
$ cd ..
```

...  and makes a secret with two values, `ssh` and and `known_hosts`.

If you look at the generated file, you'll see that the values are essentially the *base64 encoded* values of the file contents provided. In this case you've used files, but in other cases you can use `--from-literal` or you can write the secrets yaml by hand and encode the values using the `base64` utility.

Now that you've got these pre-requisits, go ahead and fill out the volumes specification.

**Next**, turn your attention to the `git-sync` container. For more information, visit the [git-sync](https://github.com/kubernetes/git-sync) github webpage.

The container spec still needs:

* `image` k8s.gcr.io/git-sync:v3.0.1
* `volumeMounts` for the `html` to `/tmp/git` and the `git-secret` to `/etc/git-secret`
* the following `env` environment variables
  * GIT_SYNC_REPO = git@github.com:WiserSolutions/kubernetes-workshop-2.git
  * GIT_SYNC_DEST = nginx
  * GIT_SYNC_SSH = "true" - the quotes are important
* `securityContext` to run as the root user

**Next**, set up the proxy, it'll be mostly a plain old Nginx container. You'll need:

* `image` nginx:alpine
* `ports` with a named container port (`http` is common) at 8080
* `volumeMounts` that mount the named configmap entry with a  `mountPath` of `/etc/nginx/nginx.conf` from the `subPath` in the volume of `nginx.conf`

**Finally**, set up `webapp` using:

* `image` nginx:alpine
* `volumeMounts` of the named `html` `emptyDir` volume written to by `git-sync` mounted to `/usr/share`.

Now you should have three *yaml* files, the pod, the configmap and the secret.


All you need now is a `Service` to make your pod discoverable and accessible. In this example, we'll use a `NodePort` service to expose the Pod outside any cluster node at a specific port over `30000`.

```yaml
kind: Service
apiVersion: v1
metadata:
  name: ws2-2
spec:
  type: NodePort
  selector:
    app: ws2-2
  ports:
  - protocol: TCP
    port: 80
    targetPort: http
```

Modify the *yaml* above in the sidecar-prox directory and the `name` and or `app` to reflect what you've used in the Pod.

**Deploy** your service!

```bash
$ kubectl create -f sidecar-proxy
secret/gitcreds created
configmap/nginx-conf created
pod/ws2-2 created
service/ws2-2 created
$ kubectl describe svc ws2-2
Name:                     ws2-2
Namespace:                default
Labels:                   <none>
Annotations:              <none>
Selector:                 app=ws2-2
Type:                     NodePort
IP:                       10.111.132.100
LoadBalancer Ingress:     localhost
Port:                     <unset>  80/TCP
TargetPort:               http/TCP
NodePort:                 <unset>  31465/TCP
Endpoints:                10.1.1.36:8080
Session Affinity:         None
External Traffic Policy:  Cluster
Events:                   <none>
```

Now try `http://localhost:31465` or whatever your `NodePort` value was.

Clean up after yourself!

```bash
$ kubectl delete -f sidecar-proxy
```

## Exercise 3 - Deployments

Deployments are really useful in that they provide an easy way to specify to Kubernetes how rollout a service and how to manage how many instances of a Pod you would like running.

This is a quick exercise where you'll copy over the yaml files from *Exercise 2* and modify the Pod file to convert the Pod to a Deployment.

The basic outline for a Deployment is like this...

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: <some name>
  labels:
    app: <label value for the app label>
spec:
  replicas: <number of replicas to run>
  selector:
    matchLabels:
      app: <the label to match>
  template:
    <your pod specification>
```

Go ahead and give it a try! Start with 2 replicas then try deleting pods or changing and applying different number os replicas and seeing how Kubernetes keeps them running.

Deployments have other properies that include time to wait for pods to be ready and what kind of deployment strategy to use. You can see more [here](https://kubernetes.io/docs/reference/generated/kubernetes-api/v1.11/#deploymentspec-v1-apps)

Clean up after yourself!

```bash
$ kubectl delete -f deployment

```bash
$ kubectl create -f deployment
```

[KubernetesForDevs](/README.md) >> workshop-2
