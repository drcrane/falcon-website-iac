#!/bin/sh
set -o errexit
set -o nounset
# set -o xtrace
if [ -f /root/firstboot.done ] ; then
	echo "firstboot.sh already complete"
	exit 0
fi
cd /root
apt-get install -y apt-transport-https curl gnupg-agent ca-certificates

# Add Kubernetes repo
curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | apt-key add -
cat <<EOF | tee /etc/apt/sources.list.d/kubernetes.list
deb https://apt.kubernetes.io/ kubernetes-xenial main
EOF

# Add Docker Repo
curl -fsSL https://download.docker.com/linux/debian/gpg | sudo apt-key add -

cat <<EOF | tee /etc/apt/sources.list.d/docker.list
deb [arch=amd64] https://download.docker.com/linux/debian buster stable
EOF

apt-get update

# Install Kubernetes
#apt-get install -y kubelet kubeadm kubectl
#apt-mark hold kubelet kubeadm kubectl

# Install conntrack (required for Kubernetes)
apt-get install -y conntrack

# Install Docker
apt-get install -y docker-ce docker-ce-cli containerd.io

# Install minikube
curl -Lo minikube https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64 \
	&& chmod +x minikube
mv minikube /usr/local/bin
#minikube start --vm-driver none --memory 1024

echo "Script was executed!" > /tmp/firstboot_done
touch /root/firstboot.done

