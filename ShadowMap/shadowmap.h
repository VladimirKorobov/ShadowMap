//
#include "common.h"

class ShadowMap
{
	GLsizei depthTextureWidth;
	GLsizei depthTextureHeight;
	GLuint depthTexture;
	GLuint depthFBO;

public:
	ShadowMap(GLsizei _depthTextureWidth, GLsizei _depthTextureHeight)
	{
		depthTextureWidth = _depthTextureWidth;
		depthTextureHeight = _depthTextureHeight;
		depthTexture = 0;
		depthFBO = 0;
	}
	~ShadowMap()
	{
		if (depthTexture != 0)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &depthTexture);
		}
		if (depthFBO != 0)
		{
			glDeleteFramebuffers(1, &depthFBO);
		}
	}

	GLuint GetDepthFBO() { return depthFBO; }
	GLuint GetDepthTexture() { return depthTexture; }
	void Clear();
	bool Prepare();
	GLuint TextureCreateDepth(GLsizei width, GLsizei height);

};