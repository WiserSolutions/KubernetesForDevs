
[KubernetesForDevs](/README.md) >> workshop-3

# Tutorial 3 - Charting Deployments with Helm

## Exercise 1 - Getting Around

Install Helm

```bash
$ helm init
$HELM_HOME has been configured at /Users/leoodonnell/.helm.

Tiller (the Helm server-side component) has been installed into your Kubernetes Cluster.

Please note: by default, Tiller is deployed with an insecure 'allow unauthenticated users' policy.
To prevent this, run `helm init` with the --tiller-tls-verify flag.
For more information on securing your installation see: https://docs.helm.sh/using_helm/#securing-your-helm-installation
Happy Helming!
```

OK Great - Let's go ahead and run some basic Helm commands

```bash
# Get the helm client and tiller versions
$ helm version
Client: &version.Version{SemVer:"v2.12.1", GitCommit:"02a47c7249b1fc6d8fd3b94e6b4babf9d818144e", GitTreeState:"clean"}
Server: &version.Version{SemVer:"v2.12.1", GitCommit:"02a47c7249b1fc6d8fd3b94e6b4babf9d818144e", GitTreeState:"clean"}

# Anything installed
$ helm ls

# Install Drupal on port 8080
$ helm install stable/drupal --set service.port=8080
NAME:   icy-dachshund
LAST DEPLOYED: Tue Jan 29 11:40:13 2019
NAMESPACE: default
STATUS: DEPLOYED

RESOURCES:
==> v1/ConfigMap
NAME                         DATA  AGE
icy-dachshund-mariadb        1     0s
icy-dachshund-mariadb-tests  1     0s

==> v1/PersistentVolumeClaim
NAME                         STATUS   VOLUME                                    CAPACITY  ACCESS MODES  STORAGECLASS  AGE
icy-dachshund-drupal-apache  Pending  hostpath                                  0s
icy-dachshund-drupal-drupal  Bound    pvc-8d7f323d-23e4-11e9-bdd7-025000000001  8Gi  RWO  hostpath  0s

==> v1/Service
NAME                   TYPE          CLUSTER-IP      EXTERNAL-IP  PORT(S)                     AGE
icy-dachshund-mariadb  ClusterIP     10.101.118.197  <none>       3306/TCP                    0s
icy-dachshund-drupal   LoadBalancer  10.102.130.189  <pending>    8080:32719/TCP,443:31193/TCP  0s

==> v1beta1/Deployment
NAME                  DESIRED  CURRENT  UP-TO-DATE  AVAILABLE  AGE
icy-dachshund-drupal  1        1        1           0          0s

==> v1beta1/StatefulSet
NAME                   DESIRED  CURRENT  AGE
icy-dachshund-mariadb  1        1        0s

==> v1/Pod(related)
NAME                                   READY  STATUS   RESTARTS  AGE
icy-dachshund-drupal-7d4475f5c4-r86p7  0/1    Pending  0         0s
icy-dachshund-mariadb-0                0/1    Pending  0         0s

==> v1/Secret
NAME                   TYPE    DATA  AGE
icy-dachshund-mariadb  Opaque  2     0s
icy-dachshund-drupal   Opaque  1     0s


NOTES:

*******************************************************************
*** PLEASE BE PATIENT: Drupal may take a few minutes to install ***
*******************************************************************

1. Get the Drupal URL:

  NOTE: It may take a few minutes for the LoadBalancer IP to be available.
        Watch the status with: 'kubectl get svc --namespace default -w icy-dachshund-drupal'

  export SERVICE_IP=$(kubectl get svc --namespace default icy-dachshund-drupal --template "{{ range (index .status.loadBalancer.ingress 0) }}{{.}}{{ end }}")
  echo "Drupal URL: http://$SERVICE_IP/"

2. Login with the following credentials

  echo Username: user
  echo Password: $(kubectl get secret --namespace default icy-dachshund-drupal -o jsonpath="{.data.drupal-password}" | base64 --decode)
```

Now note the instructions under `NOTES`. These are part of a Helm chart that provide useful information to the installer to test or access the installed application.

Go visit the drupal site and enter the password.

Now try installing Drupal again using different ports - e.g. 9090 then listing your releases.

```bash
$ helm ls
NAME            REVISION        UPDATED                         STATUS          CHART           APP VERSION     NAMESPACE
icy-dachshund   1               Tue Jan 29 11:47:35 2019        DEPLOYED        drupal-3.0.4    8.6.7           default
wayfaring-bunny 1               Tue Jan 29 11:51:37 2019        DEPLOYED        drupal-3.0.4    8.6.7           default
```

You see, that you can use the same package to install multiple versions. Even in the same namespace.

Now upgrade one of your releases. Take a look at the chart page [here](https://hub.helm.sh/charts/stable/drupal) to see what's available. Let's change the service port and upgrade...

```bash
$ helm upgrade wayfaring-bunny stable/drupal --set service.port=9080
...
$ helm ls
...
```

Now get the relese history

```bash
$ helm history wayfaring-bunny
REVISION        UPDATED                         STATUS          CHART           DESCRIPTION
1               Tue Jan 29 11:51:37 2019        SUPERSEDED      drupal-3.0.4    Install complete
2               Tue Jan 29 12:02:36 2019        DEPLOYED        drupal-3.0.4    Upgrade complete
```

Finally, let's rollback the last release and verify that the port was reverted

```bash
$ helm rollback wayfaring-bunny 1
Rollback was a success! Happy Helming!
$ kubectl get svc --namespace default wayfaring-bunny-drupal
NAME                     TYPE           CLUSTER-IP      EXTERNAL-IP   PORT(S)                        AGE
wayfaring-bunny-drupal   LoadBalancer   10.111.250.82   localhost     9090:31884/TCP,443:31089/TCP   54m
```

Now Clean up 

```bash
$ helm delete wayfaring-bunny --purge
$ helm delete icy-dachshund --purge

## Exercise 2 - Create a Basic Chart

Creating charts is pretty easy. You can start with a default template that will deploy an Nginx instance with a `Deployment`, `Service` and optional `Ingress`

Lets do it

```bash
$ helm create my-chart
```

It creates a working helm chart. Let's install it in a named release...

```bash
$ helm install --name my-first-chart ./my-chart
```

Take a look at what was created in the Dashboard, then let's look at the files

```bash
.
└── my-chart
    ├── Chart.yaml
    ├── charts
    ├── templates
    │   ├── NOTES.txt
    │   ├── _helpers.tpl
    │   ├── deployment.yaml
    │   ├── ingress.yaml
    │   ├── service.yaml
    │   └── tests
    │       └── test-connection.yaml
    └── values.yaml
```

Next let's update the chart in some way

## Exercise 3 - Leverage Other Charts


[KubernetesForDevs](/README.md) >> workshop-3
