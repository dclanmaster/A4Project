all : 
	cd hello_world; \
	$(MAKE) CC="$(CC)"
	cd nbody; \
	$(MAKE) CC="$(CC)"
	cd rings; \
	$(MAKE) CC="$(CC)" GRAPHICS_CFLAGS="$(GRAPHICS_CFLAGS)" GRAPHICS_INCLUDES="$(GRAPHICS_INCLUDES)" GRAPHICS_LDFLAGS="$(GRAPHICS_LDFLAGS)" GRAPHICS_LIBS="$(GRAPHICS_LIBS)"
	cd torii; \
	$(MAKE) CC="$(CC)" GRAPHICS_CFLAGS="$(GRAPHICS_CFLAGS)" GRAPHICS_INCLUDES="$(GRAPHICS_INCLUDES)" GRAPHICS_LDFLAGS="$(GRAPHICS_LDFLAGS)" GRAPHICS_LIBS="$(GRAPHICS_LIBS)"
	cd bunny; \
	$(MAKE) CC="$(CC)" GRAPHICS_CFLAGS="$(GRAPHICS_CFLAGS)" GRAPHICS_INCLUDES="$(GRAPHICS_INCLUDES)" GRAPHICS_LDFLAGS="$(GRAPHICS_LDFLAGS)" GRAPHICS_LIBS="$(GRAPHICS_LIBS)"



clean : 
	cd hello_world; \
	$(MAKE) clean
	cd nbody; \
	$(MAKE) clean
	cd rings; \
	$(MAKE) clean
	cd torii; \
	$(MAKE) clean
	cd bunny; \
	$(MAKE) clean



