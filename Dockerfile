FROM nginx
COPY pages /usr/share/nginx/html
COPY nginx.conf /etc/nginx/nginx.conf
