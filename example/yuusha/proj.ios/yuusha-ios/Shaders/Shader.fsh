//
//  Shader.fsh
//  yuusha-ios
//
//  Created by jukka on 16/7/30.
//  Copyright © 2016年 jukka. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
