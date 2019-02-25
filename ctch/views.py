from django.shortcuts import render
from django.template import loader
from django.http import HttpResponse,HttpResponseRedirect
from django.http import StreamingHttpResponse
from django.core.urlresolvers import reverse
from os import system
import os
import time
import random
# Create your views here.

def tool(request):
    template = loader.get_template('ctch/tool.html')
    context={}
    return HttpResponse(template.render(context, request))

def generate_name():
    unique=str(int(time.time()))+''.join(random.sample('AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789',20))
    return ('tmp/model'+unique+'.txt','tmp/result'+unique+'.txt')

def get_context(filename):
    result=dict()
    f=open(filename)
    result['parameter']=f.readline().strip('\n')
    result['constraint']=f.readline().strip('\n')
    result['t']=f.readline().strip('\n')
    result['tuple']=f.readline().strip('\n')
    result['size']=f.readline().strip('\n')
    result['time']=f.readline().strip('\n')
    content=f.readlines()
    f.close()
    for i in range(len(content)):
        content[i]=content[i].strip('\n').split(',')
    head=list()
    for i  in range(int(result['parameter'])):
        head.append('p'+str(i))
    head.append('fitness')
    context={'result':result,'content':content,'head':head,'filename':filename}
    return context

def trial(request):
    data=request.POST
    (modelname,filename)=generate_name()
    #creat input file for generation
    f=open(modelname,'w')
    f.write(data['model'])
    f.write('\n')
    f.write(data['constraints'])
    f.close()
    command='ctch '+modelname+' ' +data['strength']+' 10 >'+filename
    system(command)
    template = loader.get_template('ctch/result.html')
    context=get_context(filename)
    return HttpResponse(template.render(context, request))

def pro(request):
    if request.method=="GET" or not 'inputfile' in request.FILES.keys() :
        return HttpResponseRedirect(reverse('tool'))
    file=request.FILES['inputfile']
    (modelname,filename)=generate_name()
    f=open(modelname,'wb+')
    for chunk in file.chunks():
        f.write(chunk)
    f.close()
    command='ctch '+modelname+' ' +str(2)+' 10 >'+filename
    system(command)
    template = loader.get_template('ctch/result.html')
    context=get_context(filename)
    return HttpResponse(template.render(context, request))

def download(request):
    # do something...
    data=request.POST
    filename=data['filename']
    if(not os.path.exists('./'+filename)):
        return HttpResponseRedirect(reverse('tool'))

    def file_iterator(file_name, chunk_size=512):
        with open(file_name) as f:
            while True:
                c = f.read(chunk_size)
                if c:
                    yield c
                else:
                    break

    response = StreamingHttpResponse(file_iterator(filename))
    response['Content-Type'] = 'application/octet-stream'
    response['Content-Disposition'] = 'attachment;filename="{0}"'.format('ca.txt')
    return response
