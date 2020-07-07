/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/ShaderBase.h"
#include "glad\glad.h"
#include "../../Common/MeshGeometry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "..\..\Common\Matrix4.h"


namespace NCL {
	namespace Rendering {
		class OGLShader : public ShaderBase
		{
		public:
			friend class OGLRenderer;
			OGLShader(const string& vertex, const string& fragment, const string& geometry = "", const string& domain = "", const string& hull = "");
			~OGLShader();

			void ReloadShader() override;

			bool LoadSuccess() const {
				return programValid == GL_TRUE;
			}

			int GetProgramID() const {
				return programID;
			}	
			
			static void	PrintCompileLog(GLuint object);
			static void	PrintLinkLog(GLuint program);



			//reference:https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
			// activate the shader
  // ------------------------------------------------------------------------
			void use()
			{
				glUseProgram(programID);
			}
			// utility uniform functions
			// ------------------------------------------------------------------------
			void setBool(const std::string& name, bool value) const
			{
				glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
			}
			// ------------------------------------------------------------------------
			void setInt(const std::string& name, int value) const
			{
				glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
			}
			// ------------------------------------------------------------------------
			void setFloat(const std::string& name, float value) const
			{
				glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
			}
			// ------------------------------------------------------------------------
			void setVec2(const std::string& name, const glm::vec2& value) const
			{
				glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
			}
			void setVec2(const std::string& name, float x, float y) const
			{
				glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
			}
			// ------------------------------------------------------------------------
			void setVec3(const std::string& name, const glm::vec3& value) const
			{
				glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
			}
			void setVec3(const std::string& name, float x, float y, float z) const
			{
				glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
			}
			// ------------------------------------------------------------------------
			void setVec4(const std::string& name, const glm::vec4& value) const
			{
				glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
			}
			void setVec4(const std::string& name, float x, float y, float z, float w)
			{
				glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
			}
			// ------------------------------------------------------------------------
			void setMat2(const std::string& name, const glm::mat2& mat) const
			{
				glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
			}
			// ------------------------------------------------------------------------
			void setMat3(const std::string& name, const glm::mat3& mat) const
			{
				glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
			}
			// ------------------------------------------------------------------------
			void setMat4(const std::string& name, const glm::mat4& mat) const
			{
				glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
			}
			void setMat4(const std::string& name, const Matrix4 modelMatrix) const
			{
				//glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
				glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, (float*)&modelMatrix);
			}




		protected:
			void	DeleteIDs();

			GLuint	programID;
			GLuint	shaderIDs[ShaderStages::SHADER_MAX];
			int		shaderValid[ShaderStages::SHADER_MAX];
			int		programValid;
		public:
			
		};
	}
}