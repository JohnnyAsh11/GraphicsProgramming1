#ifndef __POSTPROCESSMANAGER_H_
#define __POSTPROCESSMANAGER_H_

#include "PostProcess.h"

/// <summary>
/// Keys for specific PostProcesses that can be applied.
/// </summary>
enum PostProcessType
{
	None,
	Blur,
	Posterization
};

/// <summary>
/// Manages all post process for the application.
/// </summary>
class PostProcessManager
{
private:
	std::unordered_map<PostProcessType, PostProcess*> m_lPostProcesses = std::unordered_map<PostProcessType, PostProcess*>();

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	std::shared_ptr<SimpleVertexShader> m_pVertexShader;
	PostProcessType m_dActivePP = PostProcessType::None;
public:
	/// <summary>
	/// Creates an instance of the PostProcessManager.
	/// </summary>
	PostProcessManager(void);

	/// <summary>
	/// Deconstructs the PostProcessManager.
	/// </summary>
	~PostProcessManager(void);

	/// <summary>
	/// Adds a PostProcess to the list.
	/// </summary>
	/// <param name="a_rPostProcess">PostProcess being added.</param>
	void AddPostProcess(PostProcessType a_dUniqueEnum, PostProcess* a_pPostProcess);
	
	/// <summary>
	/// Clears the vector list of PostProcesses.
	/// </summary>
	void ClearPostProcesses(void);

	/// <summary>
	/// Calls the PostProcess object's PreRender methods.
	/// </summary>
	void PreRender(float a_fBackgroundColor[4]);

	/// <summary>
	/// Calls the PostProcess object's PostRender methods.
	/// </summary>
	void PostRender(void);

	/// <summary>
	/// Sets the active post process within the manager.
	/// </summary>
	void SetActiveProcess(PostProcessType a_dPostProcess);

	/// <summary>
	/// Reinitializes the post process effects for the new window size.
	/// </summary>
	void OnResize(void);
};

#endif //__POSTPROCESSMANAGER_H_

