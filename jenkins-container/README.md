# Build and run on Debian

Run base image and manually configure everything:

    docker run --name jenkins -i -t -p 8080:8080 \
        -v /var/run/docker.sock:/var/run/docker.sock \
        -v ${PWD}/jenkins_home:/var/jenkins_home \
        jenkins/jenkins:2.249.1-lts-alpine

Partial pre-configuration:

    docker build --tag jenkins:custom .

    docker run --name jenkins -i -t -p 8080:8080 -v \
        /var/run/docker.sock:/var/run/docker.sock -v \
        ${PWD}/jenkins_home:/var/jenkins_home jenkins:custom


