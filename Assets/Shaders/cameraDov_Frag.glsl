#version 330 core
uniform sampler2D mainTex;
uniform int texWide;
uniform int texHight;
uniform int lens;
uniform int IsUseFieldOfDepth;


in Vertex
{	
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColor;


//	fragColor.rgb = vec3(texture(mainTex,IN.texCoord).a);

/*       本例中使用的算子包含了12个数据，记录的是纹理的偏移值，利用这一偏移数据，获取当前像素的周围12个像素，
并计算这12个像素的平均值，最终得到一张模糊的图像。需要注意的是，偏移算子本身记录的是绝对值，我们需要除以屏幕的大小，
以排除屏幕大小对偏移效果的影响。之后，根据模糊因子(取值范围在0~1) 之间，在原图像和模糊图像之间进行插值，得到最终的数据。

reference https://blog.csdn.net/ZJU_fish1996/article/details/82952866
*/
	void main(void) {

if(IsUseFieldOfDepth==0){
              
   vec4 originalColour = texture(mainTex, IN.texCoord).rgba;
            fragColor = vec4(originalColour.rgb,1.0f);
        }
        else{
           


                      int kernelNum = 12;
            vec2 TwelveKernelBase[] =
            {
                {1.0,0.0},{0.5,0.866},{-0.5,0.866},
                {-1.0,0.0},{-0.5,-0.866},{0.5,-0.866},
                {1.5,0.866},{0, 1.732},{-1.5,0.866},
                {-1.5,0.866},{0,-1.732},{1.5,-0.866},
            };
            
                vec4 originalColour = texture(mainTex, IN.texCoord).rgba;
            vec2 screenSize=vec2(texWide,texHight);
                vec2 v4ScreenSize = screenSize / 3;
                vec3 Blurred = vec3(0, 0, 0);
                for(int i = 0; i < kernelNum; i++)
                {
                vec2 Offset = vec2(TwelveKernelBase[i].x / v4ScreenSize.x,
            TwelveKernelBase[i].y / v4ScreenSize.y);
                vec4 currentColour = texture(mainTex, IN.texCoord + Offset);
                Blurred += mix(originalColour.rgb, currentColour.rgb, originalColour.a);
            }
            fragColor = vec4 (Blurred / kernelNum , 1.0f);


        }

	
}
