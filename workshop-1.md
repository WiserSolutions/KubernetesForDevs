
[KubernetesForDevs](/README.md) >> workshop-1

# Workshop 1 - Getting Around In Kubernetes

This workshop focuses on the very basics of getting around in Kubernetes by introducing you to the *kubectl* ([how to pronounce](http://www.howtopronounce.cc/kubectl)) the workhorse tool between you and the Kubernetes API and then the [Kubernetes Dashboard](https://github.com/kubernetes/dashboard).

## Intro to *kubectl*

You should already have *kubectl* installed and working. If you don't have it installed (try running `which kubectl` to be sure), see [Install and Set Up kubectl](https://kubernetes.io/docs/tasks/tools/install-kubectl/). You may also want to bookmard the [kubectl cheat sheet](https://kubernetes.io/docs/reference/kubectl/cheatsheet/)

Check that its running properly - you should see something like the following

```bash
$ kubectl version
Client Version: version.Info{Major:"1", Minor:"13", GitVersion:"v1.13.0", GitCommit:"ddf47ac13c1a9483ea035a79cd7c10005ff21a6d", GitTreeState:"clean", BuildDate:"2018-12-03T21:04:45Z", GoVersion:"go1.11.2", Compiler:"gc", Platform:"darwin/amd64"}
Server Version: version.Info{Major:"1", Minor:"13", GitVersion:"v1.13.0", GitCommit:"ddf47ac13c1a9483ea035a79cd7c10005ff21a6d", GitTreeState:"clean", BuildDate:"2018-12-03T20:56:12Z", GoVersion:"go1.11.2", Compiler:"gc", Platform:"linux/amd64"}
```

If this worked you should see both the client and server versions. 

You are able to connect to the server because you have *credentials*. There are several ways to connect to Kubernetes with *credentials*. One is via *bearer tokens*, another is through *X509 client certs*. These are found in `$HOME/.kube/config`. You can use your favorite editor to take a look at your *config* or try `kubectl config view` to see it now.

If you have installed via *Docker CE* you should see *yaml* file with entries for `docker-desktop` including both the cluster's CA and, key and certificate. This information is called a *context*.

*kubectl* has a number of commands that you can use to setup, modify or select a *context*. Enter the following command to see these now.

```bash
$ kubectl config
```
If you have installed Kubernetes throush *Docker CE*, you may also select the *context* via the `system menu bar > Docker > Kubernetes > Context`.

## *kubectl get*-ing Around

Kubernetes is essentially a big state machine that is attempting to bring all of its stored object to their [Desired State](https://kubernetes.io/docs/concepts/). There are quite a few objects that you can get the state for. Let's get a list...

`$ kubectl api-resources`

Now spend a few minutes `get`-ing some of these resources.

Start with *namespaces*, you either name a *namespace* where a resource is found, or you specifiy `--all-namespaces`

1. Get all of the namespaces `kubectl get namespaces`. Note that some are *namespaced*, like `services` and `pods` and others, like `nodes` and `namespaces` are not. You may also note the *shortname* codes that you can use.
2. Get all of the pods in all of the namespaces `kubectl get pods --all-namespaces`
3. Get just the pods in *kube-system* and show more detailed information `kubectl -n kube-system get pods -o wide`
4. Now go and try this for some other types of objects like, `nodes`, `deployments`, `services`, `endpoints` and a few others

Once you've identified a resource, you can get more information with the `describe` verb. Get a description of the `docker-desktop` *node*

`$ kubectl describe docker-desktop`

Now go and get descriptions of a few other things: *deployments*, *services* etc

## Creating a few things

Now that you've looked around a bit, let's go and create some things. The Kubernetes API offers REST endpoints, so you can *CREATE*, *DELETE*, *PATCH* and *LIST*. You do this through *kubectl* with the verbs *create*, *delete*, *apply* and *get*.

Lets add a simple *NGINX* server in our own namespace...

1. Create the namespace - `kubectl create namespace <yourname>`
2. Create the *Deployment* - `kubectl -n <yourname> create deployment --image nginx my-nginx`
3. List the pods and deployment - `kubectl -n <yourname> get pods`, `kubectl -n <yourname> get deployment`
4. Scale it up - `kubectl -n <yourname> scale deployment --replicas 2 my-nginx`
5. Check the number of pods - `kubectl -n <yourname> get pods`
6. Expose the service - `kubectl -n <yourname> expose deployment my-nginx --port=80 --type=LoadBalancer`
7. Check that it is exposed - `kubectl -n <yourname> get svc -o wide`

Now you should have something like

```bash
NAME       TYPE           CLUSTER-IP    EXTERNAL-IP   PORT(S)        AGE    SELECTOR
my-nginx   LoadBalancer   10.99.11.10   localhost     80:30505/TCP   116s   app=my-nginx
```
If you had deployed this to a cloud provider, the external IP would be a loadbalancer. Try hitting this endpoint...

```bash
$ curl localhost
<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
...
</html>
```

Typing commands by hand is great, but not really something you'll want to be doing as part of any repeatable workflow. You can also work with *yaml* or *json* files.

Let's go and grab the yaml from the deployment and service we created.

```bash
kubectl -n <yourname> get -o yaml --export svc/my-nginx deployment/my-nginx > nginx.yaml
```

Now let's go and apply this yaml

```bash
kubectl -n <yourname> apply -f nginx.yaml
```

You may see a warning because the objects were created by hand, but this is fine.

Now go and edit the yaml and look for `replicas`. Change the value to `4` and rerun the `apply`. Once this is done, let's inspect the results in the cluster.

```bash
$ kubectl -n <yourname> get pods
```

Let's tail the logs.

```bash
$ for i in {1..10}; do curl localhost; done
$ kubectl -n <yourname> logs -l app=my-nginx
192.168.65.3 - - [15/Jan/2019:17:02:36 +0000] "GET / HTTP/1.1" 200 612 "-" "curl/7.54.0" "-"
192.168.65.3 - - [15/Jan/2019:17:02:37 +0000] "GET / HTTP/1.1" 200 612 "-" "curl/7.54.0" "-"
...
```

The example above, uses the *label* `app=ny-nginx` (you can find it in the yaml) to identify all of the pods to get the logs. You can also use the `-f` option to *follow* a log as in `tail -f`, but for this you'll need to specify the pod name specifically.

```bash
$ kubctl -n <yourname> logs -f my-nginx-6cc48cd8db-bsbtt
192.168.65.3 - - [15/Jan/2019:17:10:37 +0000] "GET / HTTP/1.1" 200 612 "-" "curl/7.54.0" "-"
192.168.65.3 - - [15/Jan/2019:17:10:37 +0000] "GET / HTTP/1.1" 200 612 "-" "curl/7.54.0" "-"
192.168.65.3 - - [15/Jan/2019:17:10:37 +0000] "GET / HTTP/1.1" 200 612 "-" "curl/7.54.0" "-"

```

## The Kubernetes Dashboard

Becoming skilled with *kubectl* is valuable, but things could be easier. Good News! There is a [Dashboard](https://github.com/kubernetes/dashboard).

Let's install the Dashboard!

```bash
kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v1.10.1/src/deploy/recommended/kubernetes-dashboard.yaml
```

Now that you've got the dashboard, you'll need to access it. It isn't a good idea to expose it directly, so we tunnel through authenticated *Kubernetes API* connection by using `kubectl proxy`.

In a new window, run the proxy...

```bash
$ kubectl proxy
Starting to serve on 127.0.0.1:8001
```

You'll also need to create a token to authenticate. Start by gettting secrets to find the name of a token that has the privileges we want. Then we can *describe* it and grab the token.

```bash
$ kubectl -n kube-system get secrets |grep token
...
attachdetach-controller-token-w5rnw              kubernetes.io/service-account-token   3      21m
...
$ kubectl -n kube-system describe secret attachdetach-controller-token-w5rnw |grep token
Name:         attachdetach-controller-token-w5rnw
Type:  kubernetes.io/service-account-token
token:      eyJhbGciOiJSUzI1NiIsImtpZCI6IiJ9.eyJpc3MiOiJrdWJlcm5ldGVzL3NlcnZpY2VhY2NvdW50Iiwia3ViZXJuZXRlcy5p...
...
KqdKfewZJARSFwdULdqbxIbIbQgRg9WwZYr2jBrji2N2u1EweCQ
```
Now you can access your dashboard. Visit the dashboard at http://localhost:8001/api/v1/namespaces/kube-system/services/https:kubernetes-dashboard:/proxy/#!/overview?namespace=default then select the `token` method of authenticate, then copy and paste your token from above. If you have a password manager, you may wish to save this value.

This should log you in and you should be seeing the `default` namespace *overview*. Take some time to look around.

### Clean Up

Once you're done, you can clean up the objects you created in <yourname> namespace by simply deleting the namespace.

```bash
$ kubectl delete namespace <yourname>
```

[KubernetesForDevs](/README.md) >> workshop-1
