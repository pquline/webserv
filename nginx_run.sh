docker build -t webserv-nginx .
docker run --name my-nginx -p 8080:80 -d webserv-nginx
