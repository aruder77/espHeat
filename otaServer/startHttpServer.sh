#!/bin/bash
docker run -d -p 8080:80 -v /Users/axel/Documents/Projects/Arduino/espHeat/otaServer/data:/usr/share/nginx/html:ro nginx:stable 
