from django.conf.urls import url
from . import views
urlpatterns = [
    url(r'^tool$', views.tool, name='tool'),
    url(r'^trial$', views.trial, name='trial'),
    url(r'^pro$', views.pro, name='pro'),
    url(r'^download$', views.download, name='download'),
]
