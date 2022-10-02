frameworks=-framework Foundation -framework Metal -framework MetalKit -framework Cocoa

exe=rasterizer2.app/Contents/MacOS/rasterizer2
metallib=rasterizer2.app/Contents/Resources/default.metallib
nib=rasterizer2.app/Contents/Resources/Base.lproj/MainMenu.nib
ibtool=/Applications/Xcode.app/Contents/Developer/usr/bin/ibtool

all: $(exe) $(metallib) $(nib)



$(exe): main.mm render_pipeline.h makefile kinect.cpp kinect.hpp
	clang++ -std=c++20 -O3 -w $(frameworks) \
		-I ../glm/include \
		-I ../libfreenect2 \
		../libfreenect2/libfreenect2.dylib \
		main.mm kinect.cpp -o $(exe)

$(metallib): shaders.metal render_pipeline.h makefile
	metal -O3 shaders.metal -o $(metallib)
	
$(nib): Base.lproj/MainMenu.xib makefile
	$(ibtool) --errors --warnings --notices --module rasterizer2 \
		--auto-activate-custom-fonts --target-device mac --minimum-deployment-target 12.3 \
		--output-format human-readable-text --compile $(nib) Base.lproj/MainMenu.xib \
		
clean:
	rm $(exe) $(metallib) $(nib)

run: all
	./rasterizer2.app/Contents/MacOS/rasterizer2
