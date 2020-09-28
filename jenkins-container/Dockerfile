FROM jenkins/jenkins:2.249.1-lts-alpine

# This is only part way there but it does work well
# It is assumed that the docker group on the host machine is id 998
# and that uid offsets are not in use... otherwise some other steps
# will be required.

USER root
RUN sh -c "cat /etc/group |awk '/^utmp/{ print $0; print \"docker:x:998:jenkins\"; next; }1' > /etc/group-" && \
	mv /etc/group- /etc/group && \
	apk add --no-cache docker-cli
USER jenkins

RUN /usr/local/bin/install-plugins.sh build-timeout credentials-binding timestamper ws-cleanup workflow-aggregator pipeline-stage-view ssh-agent git generic-webhook-trigger docker-workflow

# Set the default username and password
#ENV JENKINS_USER admin
#ENV JENKINS_PASS admin

# Skip the setup wizard
#ENV JAVA_OPTS -Djenkins.install.runSetupWizard=false

# Start-up scripts to set number of executors and creating the admin user
#COPY executors.groovy /usr/share/jenkins/ref/init.groovy.d/
#COPY default-user.groovy /usr/share/jenkins/ref/init.groovy.d/



