# Falken Deployment Scripts for Azure Kubernetes Service

The permissions for the vault reqire some careful investigation.

# Incomplete Repository

Some files are missing, this is because they contain secret information like
private keys and access tokens that should not be placed on github.

# Variables

for the three variables in variables.tf I use environment vars:

    export TF_VAR_client_id="---"
    export TF_VAR_client_secret="---"
    export TF_VAR_application_name="falcon-website"

