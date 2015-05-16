#pragma once

#ifndef _SKIN_PREVIEW_LISTENER_H_
#define _SKIN_PREVIEW_LISTENER_H_

class CustomImageSkin ;

class SkinPreviewListener
{
public:
	virtual void OnConfirmUseSkin(CustomImageSkin * skin) = 0 ;
	virtual void OnCancelUseSkin(CustomImageSkin * skin) = 0 ;
};

#endif
