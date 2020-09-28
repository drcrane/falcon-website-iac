provider "kubernetes" {
	load_config_file = false

	host = azurerm_kubernetes_cluster.default.kube_config.0.host
	username = azurerm_kubernetes_cluster.default.kube_config.0.username
	password = azurerm_kubernetes_cluster.default.kube_config.0.password

	client_certificate = base64decode(azurerm_kubernetes_cluster.default.kube_config.0.client_certificate)
	client_key = base64decode(azurerm_kubernetes_cluster.default.kube_config.0.client_key)
	cluster_ca_certificate = base64decode(azurerm_kubernetes_cluster.default.kube_config.0.cluster_ca_certificate)
}

resource "kubernetes_deployment" "falcon-website-app" {
	metadata {
		name = "${var.application_name}-app"
		labels = {
			App = "FalconWebsite"
		}
	}

	spec {
		replicas = 2
		selector {
			match_labels = {
				App = "FalconWebsite"
			}
		}
		template {
			metadata {
				labels = {
					App = "FalconWebsite"
				}
			}
			spec {
				container {
					image = "drcrane/falcon-website:2-927d30c"
					name  = "falcon-website-container"
	
					port {
						container_port = 8080
					}
	
					port {
						container_port = 8443
					}
	
					env {
						name = "PORT"
						value = 8080
					}
	
					env {
						name = "SSLPORT"
						value = "8443"
					}
	
					liveness_probe {
						http_get {
							path = "/nginx_status"
							port = 8080
						}
						initial_delay_seconds = 30
						period_seconds = 10
					}

					resources {
						limits {
							cpu = "0.5"
							memory = "512Mi"
						}
						requests {
							cpu    = "250m"
							memory = "50Mi"
						}
					}
				}
			}
		}
	}
}

resource "kubernetes_service" "falcon-website-service" {
	metadata {
		name = "${var.application_name}-lb-service"
	}
	spec {
		selector = {
			App = kubernetes_deployment.falcon-website-app.spec.0.template.0.metadata[0].labels.App
		}
		port {
			name = "http-port"
			port = 80
			target_port = 8080
		}
		port {
			name = "http-ssl-port"
			port = 443
			target_port = 8443
		}

		type = "LoadBalancer"
	}
}

output "lb_ip" {
	value = kubernetes_service.falcon-website-service.load_balancer_ingress[0].ip
}

