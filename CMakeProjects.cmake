if (BUILD_EXAMPLE)
	subdirs (Samples/HelloWorld)	
	subdirs (Samples/Sponza)
	subdirs (Samples/TankScene)
	subdirs (Samples/Audio)	
	subdirs (Samples/SkinnedMesh)
	subdirs (Samples/LuckyDraw)
	subdirs (Samples/DrawPrimitives)
	if (NOT BUILD_EMSCRIPTEN)
		subdirs (Samples/Materials)
		subdirs (Samples/Shader)		
		subdirs (Samples/LightmapUV)
		subdirs (Samples/Lightmapping)
		subdirs (Samples/LightmappingVertex)
		subdirs (Samples/Particles)
		subdirs (Samples/ParticlesVortex)
		subdirs (Samples/ParticlesExplosion)
		subdirs (Samples/ParticlesMagicSkill)
		subdirs (Samples/Noise2D)
		subdirs (Samples/Noise3D)
	endif()	
endif()