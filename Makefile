# LDFLAGS  = glad/src/glad.o -Llib32 -lglfw -lGL -ldl -lfreetype
# CXXFLAGS = -g -I/usr/include/freetype2 -Wall -Wno-write-strings -Wno-parentheses -DLINUX -DUSE_FREETYPE
# OBJS = shader.o gpuProgram.o linalg.o wavefront.o renderer.o gbuffer.o font.o axes.o

LDFLAGS  = -Llib32 -lglfw -lGL -ldl
CXXFLAGS = -g -Wall -Wno-write-strings -Wno-parentheses -DLINUX
OBJS = shader.o gpuProgram.o linalg.o wavefront.o renderer.o gbuffer.o axes.o glad/src/glad.o 

PROG = shader


$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS) 

clean:
	rm -f $(OBJS) *~ $(PROG)

depend:	
	makedepend -Y *.h *.cpp

# DO NOT DELETE

axes.o: linalg.h gpuProgram.h headers.h glad/include/glad/glad.h
axes.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h
gpuProgram.o: headers.h glad/include/glad/glad.h
gpuProgram.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
headers.o: glad/include/glad/glad.h glad/include/KHR/khrplatform.h
headers.o: include/GLFW/glfw3.h linalg.h
renderer.o: wavefront.h headers.h glad/include/glad/glad.h
renderer.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
renderer.o: seq.h shadeMode.h gpuProgram.h gbuffer.h
seq.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
seq.o: include/GLFW/glfw3.h linalg.h
shader.o: linalg.h
wavefront.o: headers.h glad/include/glad/glad.h
wavefront.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefront.o: seq.h shadeMode.h gpuProgram.h
axes.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
axes.o: include/GLFW/glfw3.h linalg.h axes.h gpuProgram.h
font.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
font.o: include/GLFW/glfw3.h linalg.h gpuProgram.h
gbuffer.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
gbuffer.o: include/GLFW/glfw3.h linalg.h gbuffer.h font.h shader.h
gpuProgram.o: gpuProgram.h headers.h glad/include/glad/glad.h
gpuProgram.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
linalg.o: linalg.h
renderer.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
renderer.o: include/GLFW/glfw3.h linalg.h renderer.h wavefront.h seq.h
renderer.o: shadeMode.h gpuProgram.h gbuffer.h shader.h
shader.o: headers.h glad/include/glad/glad.h glad/include/KHR/khrplatform.h
shader.o: include/GLFW/glfw3.h linalg.h wavefront.h seq.h shadeMode.h
shader.o: gpuProgram.h renderer.h gbuffer.h shader.h font.h axes.h
wavefront.o: headers.h glad/include/glad/glad.h
wavefront.o: glad/include/KHR/khrplatform.h include/GLFW/glfw3.h linalg.h
wavefront.o: gpuProgram.h wavefront.h seq.h shadeMode.h
