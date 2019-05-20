from django.conf.urls import url

from . import consumer

websocket_urlpatterns = [
    url(r'^ws/chat/(?P<room_name>[^/]+)/$',consumer.ChatConsumer),

]