#version 330 core
in vec2 fragCoord;
out vec4 fragColor;

/*
* 常量定义
*/
// uniform vec3 iResolution;   // 窗口分辨率，单位像素
// uniform float iTime;        // 程序运行时间，单位秒
// uniform float iTimeDelta;   // 渲染时间，单位秒
// uniform float iFream;       // 帧率
// uniform vec4 iMouse;        // 鼠标位置
// uniform vec4 iDate;         // 日期（年月日时）

#define MAX_STEPS 100
#define MAX_DIST 100.
#define SURF_DIST .01

float GetDist(vec3 p)
{
    vec4 s=vec4(0,1,6,1);// A sphere's pos.xyzr
    
    float sphereDist=length(p-s.xyz)-s.w;
    float planeDist=p.y;
    
    float d=min(sphereDist,planeDist);
    return d;
}

float RayMarch(vec3 ro,vec3 rd)
{
    float dO=0.;
    
    for(int i=0;i<MAX_STEPS;i++){
        vec3 p=ro+rd*dO;
        float dS=GetDist(p);
        dO+=dS;
        if(dO>MAX_DIST||dS<SURF_DIST)break;
    }
    return dO;
}

vec3 GetNormal(vec3 p)
{
    float d=GetDist(p);
    vec2 e=vec2(.01,0);
    
    vec3 n=d-vec3(
        GetDist(p-e.xyy),
        GetDist(p-e.yxy),
        GetDist(p-e.yyx));
    return normalize(n);
}

float GetLight(vec3 p)
{
    vec3 lightPos=vec3(0,5,6);// a light's pos.xyz
    //lightPos.xz+=vec2(sin(iTime),cos(iTime))*2.;
    vec3 l =normalize(lightPos-p);
    vec3 n=GetNormal(p);
    
    float dif=clamp(dot(n,l),0.,1.);
    float d=RayMarch(p+n*SURF_DIST*2.,l);
    if(d<length(lightPos-p)) dif*=.1;
    
    return dif;
}

void main()
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 iResolution2=vec2(1920,1080);
    vec2 uv = (fragCoord-.5*iResolution2.xy)/iResolution2.y;
    //uv=fragCoord/iResolution2;
    
    // Time varying pixel color
    vec3 col = vec3(0);
    
    vec3 ro=vec3(0,1,0);
    vec3 rd=normalize(vec3(uv.x,uv.y,1));
    
    float d=RayMarch(ro,rd);
    
    vec3 p=ro+rd*d;
    float dif=GetLight(p);
    col=vec3(dif);

    // Output to screen
    fragColor = vec4(col,1.0);
}
