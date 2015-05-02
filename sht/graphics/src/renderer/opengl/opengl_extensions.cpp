#include "opengl_extensions.h"

// In x32 and x64 there are same namings
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

namespace gl {

	bool GetPointers(void)
	{
		// use standart names
		ClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glClientActiveTexture");
		ActiveTextureARB = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
		GenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

		// try with ARB postfix
		if (!ClientActiveTextureARB ||
			!ActiveTextureARB)
		{
			ClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
			ActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
		}
		if (!ClientActiveTextureARB ||
			!ActiveTextureARB ||
			!GenerateMipmapEXT)
			return false;

		return true;
	}
	bool GetShaderPointers(void)
	{
		DeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
		GetHandleARB = (PFNGLGETHANDLEARBPROC)wglGetProcAddress("glGetHandleARB");
		DetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress("glDetachObjectARB");
		CreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
		ShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
		CompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
		CreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
		AttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
		LinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
		ValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)wglGetProcAddress("glValidateProgramARB");
		UseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
		GetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
		GetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
		Uniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
		Uniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
		Uniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
		Uniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
		Uniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
		Uniform1fvARB = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");
		Uniform2fvARB = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress("glUniform2fvARB");
		Uniform3fvARB = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress("glUniform3fvARB");
		Uniform4fvARB = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress("glUniform4fvARB");
		UniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)wglGetProcAddress("glUniformMatrix2fvARB");
		UniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)wglGetProcAddress("glUniformMatrix3fvARB");
		UniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress("glUniformMatrix4fvARB");
		GetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
		BindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)wglGetProcAddress("glBindAttribLocationARB");
		GetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)wglGetProcAddress("glGetAttribLocationARB");
		VertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)wglGetProcAddress("glVertexAttribPointerARB");
		EnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glEnableVertexAttribArrayARB");
		DisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glDisableVertexAttribArrayARB");

		if (!DeleteObjectARB ||
			!GetHandleARB ||
			!DetachObjectARB ||
			!CreateShaderObjectARB ||
			!ShaderSourceARB ||
			!CompileShaderARB ||
			!CreateProgramObjectARB ||
			!AttachObjectARB ||
			!LinkProgramARB ||
			!ValidateProgramARB ||
			!UseProgramObjectARB ||
			!GetObjectParameterivARB ||
			!GetInfoLogARB ||
			!Uniform1fARB ||
			!Uniform2fARB ||
			!Uniform3fARB ||
			!Uniform4fARB ||
			!Uniform1iARB ||
			!Uniform1fvARB ||
			!Uniform2fvARB ||
			!Uniform3fvARB ||
			!Uniform4fvARB ||
			!UniformMatrix2fvARB || !UniformMatrix3fvARB || !UniformMatrix4fvARB ||
			!GetUniformLocationARB ||
			!BindAttribLocationARB ||
			!GetAttribLocationARB ||
			!VertexAttribPointerARB ||
			!EnableVertexAttribArrayARB ||
			!DisableVertexAttribArrayARB)
			return false;

		return true;
	}
	bool GetVBOPointers(void)
	{
		BindBufferARB = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
		BufferDataARB = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
		BufferSubDataARB = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
		DeleteBuffersARB = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
		GenBuffersARB = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
		MapBufferARB = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
		UnmapBufferARB = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");

		if (!BindBufferARB ||
			!BufferDataARB ||
			!BufferSubDataARB ||
			!DeleteBuffersARB ||
			!GenBuffersARB ||
			!MapBufferARB ||
			!UnmapBufferARB)
		{
			BindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
			BufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
			BufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
			DeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
			GenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
			MapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
			UnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
		}
		if (!BindBufferARB ||
			!BufferDataARB ||
			!BufferSubDataARB ||
			!DeleteBuffersARB ||
			!GenBuffersARB ||
			!MapBufferARB ||
			!UnmapBufferARB)
			return false;

		return true;
	}
	bool GetFBOPointers(void)
	{
		BindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
		GenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
		DeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
		IsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
		BindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
		GenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
		DeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
		IsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
		RenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
		FramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
		FramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
		FramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
		FramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
		CheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
		DrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)wglGetProcAddress("glDrawBuffersARB");

		if (!BindFramebufferEXT ||
			!GenFramebuffersEXT ||
			!DeleteFramebuffersEXT ||
			!IsFramebufferEXT ||
			!BindRenderbufferEXT ||
			!GenRenderbuffersEXT ||
			!DeleteRenderbuffersEXT ||
			!IsRenderbufferEXT ||
			!RenderbufferStorageEXT ||
			!FramebufferRenderbufferEXT ||
			!FramebufferTexture1DEXT ||
			!FramebufferTexture2DEXT ||
			!FramebufferTexture3DEXT ||
			!CheckFramebufferStatusEXT ||
			!DrawBuffersARB)
			return false;

		return true;
	}
	bool IsExtSupported(const char* szTargetExtension)
	{
		const unsigned char *pszExtensions = NULL;
		const unsigned char *pszStart;
		unsigned char *pszWhere, *pszTerminator;

		// Extension names should not have spaces
		pszWhere = (unsigned char *)strchr(szTargetExtension, ' ');
		if (pszWhere || *szTargetExtension == '\0')
			return false;

		// Get Extensions String
		pszExtensions = glGetString(GL_EXTENSIONS);

		// Search The Extensions String For An Exact Copy
		pszStart = pszExtensions;
		for (;;)
		{
			pszWhere = (unsigned char *)strstr((const char *)pszStart, szTargetExtension);
			if (!pszWhere)
				break;
			pszTerminator = pszWhere + strlen(szTargetExtension);
			if (pszWhere == pszStart || *(pszWhere - 1) == ' ')
				if (*pszTerminator == ' ' || *pszTerminator == '\0')
					return true;
			pszStart = pszTerminator;
		}
		return false;
	}

} // namespace gl