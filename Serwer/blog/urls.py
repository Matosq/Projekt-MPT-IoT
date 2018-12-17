from django.urls import path, include
from . import views

urlpatterns = [
    path('', views.home, name='blog-home'),
    path('analysis/', views.analysis, name='blog-analysis'),
    path('about/', views.about, name='blog-about'),
    path('charts/', views.chosen_chart_view, name='chosen_chart_view')
]
