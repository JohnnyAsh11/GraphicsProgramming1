#ifndef __MATERIAL_H_
#define __MATERIAL_H_

#include <memory>
#include "SimpleShader.h"

class Material
{
private:


public:
	Material(std::shared_ptr<SimpleVertexShader> a_pVertexShader, std::shared_ptr<SimplePixelShader> a_pPixelShader);


};

#endif //__MATERIAL_H_

