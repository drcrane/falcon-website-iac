# Kubernetes yaml for falcon-website

Here are the native yaml files that can be used by kubectl (and minikube) to
create a local instance of the falken website. The site is largely the same as
the Azure version.

Apply the deployment, this will create two pods running the application
on the local machine:

    minikube kubectl -- apply -f falcon-website-deployment.yaml

To access the application apply a service, the two provided here are
LoadBalancer and NodePort. They can be applied with the YAML but also by using
the kubectl command directly:

List current deployments:

    minikube kubectl get deployment

List current services:

    minikube kubectl get service

Create a new NodePort service:

    kubectl expose deployment/falcon-website --type "NodePort" --port 8080

List the services again and use curl to send a request to the containers:

    curl http://10.107.209.237:8080/

The ip address is only available in the minikube cluster but the exposed port
is available on the VM's (or host if using --vm-driver=none) IP address.

`falcon-website-nodeport.yaml` uses a fixed port number and so allows reverse
proxying by nginx for the example site.

    minikube kubectl -- apply -f falcon-website-nodeport.yaml

To use a LoadBalancer with minikube seems to require a bit of a
[hack](https://blog.codonomics.com/2019/02/loadbalancer-support-with-minikube-for-k8s.html)

    minikube kubectl -- patch service falcon-website-service-lb -p '{"spec": {"type": "LoadBalancer", "externalIPs":["192.168.80.28"]}}'

In this case, as minikube is running without a VM (--vm-driver=none)
192.168.80.28 is the IP address of eth0 so that works well but if minikube is
running inside a VM the IP address of the VM can be discovered like this:

    minikube ip

See more detail on the service just patched:

    minikube kubectl -- get service/falcon-website-service-lb --output json

[https://v1-18.docs.kubernetes.io/docs/tasks/manage-kubernetes-objects/update-api-object-kubectl-patch/](https://v1-18.docs.kubernetes.io/docs/tasks/manage-kubernetes-objects/update-api-object-kubectl-patch/)

I think that this kind of operation is [not supported](https://kubernetes.io/docs/tutorials/stateless-application/expose-external-ip-address/).


