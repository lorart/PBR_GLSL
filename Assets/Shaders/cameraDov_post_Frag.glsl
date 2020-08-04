precision mediump int;
precision mediump float;
#endif
 
varying vec4 v_color;
varying vec2 v_texcoord;
uniform sampler2D texture;
uniform vec2 screenSize;
 
int kernelNum = 12;
vec2 g_v2TwelveKernelBase[] =
{
    {1.0,0.0},{0.5,0.866},{-0.5,0.866},
    {-1.0,0.0},{-0.5,-0.866},{0.5,-0.866},
    {1.5,0.866},{0, 1.732},{-1.5,0.866},
    {-1.5,0.866},{0,-1.732},{1.5,-0.866},
};
 
void main()
{
    vec4 v4Original = texture2D(texture, v_texcoord);
 
    vec2 v4ScreenSize = screenSize / 5;
    vec3 v3Blurred = vec3(0, 0, 0);
    for(int i = 0; i < kernelNum; i++)
    {
       vec2 v2Offset = vec2(g_v2TwelveKernelBase[i].x / v4ScreenSize.x,
 g_v2TwelveKernelBase[i].y / v4ScreenSize.y);
       vec4 v4Current = texture2D(texture, v_texcoord + v2Offset);
       v3Blurred += lerp(v4Original.rgb, v4Current.rgb, v4Original.a);
   }
   gl_FragColor = vec4 (v3Blurred / kernelNum , 1.0f);
}
  