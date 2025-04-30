#ifndef __POSTPROCESSMANAGER_H_
#define __POSTPROCESSMANAGER_H_

#include "PostProcess.h"
#include <vector>

/// <summary>
/// Manages all post process for the application.
/// </summary>
class PostProcessManager
{
private:
	std::vector<PostProcess*> m_lPostProcesses = std::vector<PostProcess*>();

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	std::shared_ptr<SimpleVertexShader> m_pVertexShader;

public:
	/// <summary>
	/// Creates an instance of the PostProcessManager.
	/// </summary>
	PostProcessManager();

	/// <summary>
	/// Deconstructs the PostProcessManager.
	/// </summary>
	~PostProcessManager();

	/// <summary>
	/// Adds a PostProcess to the list.
	/// </summary>
	/// <param name="a_rPostProcess">PostProcess being added.</param>
	void AddPostProcess(PostProcess* a_pPostProcess);
	
	/// <summary>
	/// Clears the vector list of PostProcesses.
	/// </summary>
	void ClearPostProcesses();

	/// <summary>
	/// Calls the PostProcess object's PreRender methods.
	/// </summary>
	void PreRender(float a_fBackgroundColor[4]);

	/// <summary>
	/// Calls the PostProcess object's PostRender methods.
	/// </summary>
	void PostRender();

	/// <summary>
	/// Reinitializes the post process effects for the new window size.
	/// </summary>
	void OnResize();
};

#endif //__POSTPROCESSMANAGER_H_

