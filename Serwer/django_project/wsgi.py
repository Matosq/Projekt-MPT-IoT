"""
WSGI config for django_project project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/2.1/howto/deployment/wsgi/
"""

import os

from django.core.wsgi import get_wsgi_application

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'django_project.settings')
os.system('wget http://launchpadlibrarian.net/212189159/libmysqlclient18_5.6.25-0ubuntu1_amd64.deb && dpkg -i libmysqlclient18_5.6.25-0ubuntu1_amd64.deb && rm libmysqlclient18_5.6.25-0ubuntu1_amd64.deb')
os.system('apt-get update && apt-get install -y mysql-server default-libmysqlclient-dev gcc && pip install mysqlclient')

application = get_wsgi_application()
