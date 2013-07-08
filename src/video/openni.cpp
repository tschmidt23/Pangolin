/* This file is part of the Pangolin Project.
 * http://github.com/stevenlovegrove/Pangolin
 *
 * Copyright (c) 2013 Steven Lovegrove
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <pangolin/video/openni.h>

namespace pangolin
{

OpenNiVideo::OpenNiVideo(OpenNiSensorType s1, OpenNiSensorType s2)
{
    sensor_type[0] = s1;
    sensor_type[1] = s2;
    
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = context.Init();
    if (nRetVal != XN_STATUS_OK) {
        std::cerr << "context.Init: " << xnGetStatusString(nRetVal) << std::endl;
    }
    
    XnMapOutputMode mapMode;
    mapMode.nXRes = XN_VGA_X_RES;
    mapMode.nYRes = XN_VGA_Y_RES;
    mapMode.nFPS = 30;
    
    sizeBytes = 0;
    
    bool use_depth = false;
    bool use_ir = false;
    bool use_rgb = false;
    bool depth_to_color = false;
    
    for(int i=0; i<2; ++i) {
        VideoPixelFormat fmt;

        // Establish output pixel format for sensor streams
        switch( sensor_type[i] ) {
        case OpenNiDepth:
        case OpenNiDepthRegistered:
        case OpenNiIr:
        case OpenNiIrProj:
            fmt = VideoFormatFromString("GRAY16LE");
            break;
        case OpenNiIr8bit:
        case OpenNiIr8bitProj:
            fmt = VideoFormatFromString("GRAY8");
            break;
        case OpenNiRgb:
            fmt = VideoFormatFromString("RGB24");
            break;
        case OpenNiUnassigned:
        default:
            continue;
        }
        
        switch( sensor_type[i] ) {
        case OpenNiDepthRegistered:
            depth_to_color = true;
        case OpenNiDepth:
            use_depth = true;
            break;
        case OpenNiIr:
        case OpenNiIr8bit:
            use_ir = true;
            break;
        case OpenNiIrProj:
        case OpenNiIr8bitProj:
            use_ir = true;
            use_depth = true;
            break;
        case OpenNiRgb:
            use_rgb = true;
            break;
        case OpenNiUnassigned:
            break;
        }

        const StreamInfo stream(fmt, mapMode.nXRes, mapMode.nYRes, (mapMode.nXRes * fmt.bpp) / 8, (unsigned char*)0 + sizeBytes);
        sizeBytes += stream.SizeBytes();
        streams.push_back(stream);
    }
    
    if( use_rgb ) {
        nRetVal = imageNode.Create(context);
        if (nRetVal != XN_STATUS_OK) {
            std::cerr << "imageNode.Create: " << xnGetStatusString(nRetVal) << std::endl;
        }else{
            nRetVal = imageNode.SetMapOutputMode(mapMode);
            if (nRetVal != XN_STATUS_OK) {
                std::cerr << "imageNode.SetMapOutputMode: " << xnGetStatusString(nRetVal) << std::endl;
            }
        }
    }
    
    if( use_depth ) {
        nRetVal = depthNode.Create(context);
        if (nRetVal != XN_STATUS_OK) {
            std::cerr << "depthNode.Create: " << xnGetStatusString(nRetVal) << std::endl;
        }else{
            nRetVal = depthNode.SetMapOutputMode(mapMode);
            if (nRetVal != XN_STATUS_OK) {
                std::cerr << "depthNode.SetMapOutputMode: " << xnGetStatusString(nRetVal) << std::endl;
            }
            if (depth_to_color && use_rgb) {
                if( depthNode.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT) ) {
                    nRetVal = depthNode.GetAlternativeViewPointCap().SetViewPoint( imageNode  );
                    if (nRetVal != XN_STATUS_OK) {
                        std::cerr << "depthNode.GetAlternativeViewPointCap().SetViewPoint(imageNode): " << xnGetStatusString(nRetVal) << std::endl;
                    }
                }
            }
        }
    }
    
    if( use_ir ) {
        nRetVal = irNode.Create(context);
        if (nRetVal != XN_STATUS_OK) {
            std::cerr << "irNode.Create: " << xnGetStatusString(nRetVal) << std::endl;
        }else{
            nRetVal = irNode.SetMapOutputMode(mapMode);
            if (nRetVal != XN_STATUS_OK) {
                std::cerr << "irNode.SetMapOutputMode: " << xnGetStatusString(nRetVal) << std::endl;
            }
        }
    }
        
    Start();
}

OpenNiVideo::~OpenNiVideo()
{
    context.Release();
}

size_t OpenNiVideo::SizeBytes() const
{
    return sizeBytes;
}

const std::vector<StreamInfo>& OpenNiVideo::Streams() const
{
    return streams;
}

void OpenNiVideo::Start()
{
    //    XnStatus nRetVal = 
    context.StartGeneratingAll();
}

void OpenNiVideo::Stop()
{
    context.StopGeneratingAll();
}

bool OpenNiVideo::GrabNext( unsigned char* image, bool wait )
{
    //    XnStatus nRetVal = context.WaitAndUpdateAll();
    XnStatus nRetVal = context.WaitAnyUpdateAll();
    //    nRetVal = context.WaitOneUpdateAll(imageNode);
    
    if (nRetVal != XN_STATUS_OK) {
        std::cerr << "Failed updating data: " << xnGetStatusString(nRetVal) << std::endl;
        return false;
    }else{
        unsigned char* out_img = image;
        
        for(int i=0; i<2; ++i) {
            switch (sensor_type[i]) {
            case OpenNiDepth:
            case OpenNiDepthRegistered:
            {
                const XnDepthPixel* pDepthMap = depthNode.GetDepthMap();
                memcpy(out_img,pDepthMap, streams[i].SizeBytes() );
                break;
            }
            case OpenNiIr:
            case OpenNiIrProj:
            {
                const XnIRPixel* pIrMap = irNode.GetIRMap();
                memcpy(out_img, pIrMap, streams[i].SizeBytes() );
                break;
            }
            case OpenNiIr8bit:
            case OpenNiIr8bitProj:
            {
                const XnIRPixel* pIr16Map = irNode.GetIRMap();

                // rescale from 16-bit (10 effective) to 8-bit
                xn::IRMetaData meta_data;
                irNode.GetMetaData(meta_data);
                int w = meta_data.XRes();
                int h = meta_data.YRes();

                XnUInt8 pIrMapScaled[w * h];
                for (int v = 0; v < h; ++v)
                for (int u = 0; u < w; ++u) {
                    int val = *pIr16Map >> 2; // 10bit to 8 bit
                    pIrMapScaled[w * v + u] = val;
                    pIr16Map++;
                }

                memcpy(out_img, pIrMapScaled, streams[i].SizeBytes() );
                break;
            }
            case OpenNiRgb:
            {
                const XnUInt8* pImageMap = imageNode.GetImageMap();
                memcpy(out_img,pImageMap, streams[i].SizeBytes());
                break;
            }
            case OpenNiUnassigned:
                break;
            }
            
            out_img += streams[i].SizeBytes();
        }
        
        return true;
    }
}

bool OpenNiVideo::GrabNewest( unsigned char* image, bool wait )
{
    return GrabNext(image,wait);
}

}

