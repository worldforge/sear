// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: LibModelFile_Loader.h,v 1.2 2005-03-15 17:55:03 simon Exp $

#ifndef SEAR_LOADERS_LIBMODELFILE_LOADER_H
#define SEAR_LOADERS_LIBMODELFILE_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
	
class LibModelFile_Loader : public ModelLoader {
public:	
  LibModelFile_Loader(ModelHandler *mh);
  ~LibModelFile_Loader();

  ModelRecord *loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);
	  
protected:
  static const std::string LIBMODELFILE;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_LIBMODELFILE_LOADER_H */
