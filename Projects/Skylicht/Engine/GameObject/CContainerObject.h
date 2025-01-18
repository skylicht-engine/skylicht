/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "CGameObject.h"

namespace Skylicht
{
	class SKYLICHT_API CContainerObject : public CGameObject
	{
	protected:
		ArrayGameObject m_childs;
		ArrayGameObject m_add;
		ArrayGameObject m_remove;

		core::array<CGameObject*> m_arrayChildObjects;

		core::map<std::wstring, CGameObject*> m_objectByName;
		core::map<std::string, CGameObject*> m_objectByID;

		bool m_updateRemoveAdd;
		bool m_updateListChild;

		int m_lastGenerateID;

	public:
		CContainerObject(CGameObject* parent, CZone* zone);

		virtual ~CContainerObject();

		CGameObject* createEmptyObject();

		CContainerObject* createContainerObject();

		CGameObject* createObject(CObjectSerializable* data, bool generateNewID);

		void updateAddRemoveObject(bool force = false);

		inline void notifyUpdateListChild()
		{
			m_updateListChild = true;
		}

		int getNumberObjects();

		void updateIndexSearchObject();

		void bringToNext(CGameObject* object, CGameObject* target, bool behind);

		void bringToChild(CGameObject* object);

		void sortChildsByTemplateOrder(std::vector<std::string>& order);

		virtual void setTemplateChanged(bool b);

		virtual CGameObject* searchObject(const wchar_t* objectName);

		virtual CGameObject* searchObjectInChild(const wchar_t* objectName);

		virtual CGameObject* searchObjectByID(const char* id);

		virtual CGameObject* searchObjectInChildByID(const char* id);

		virtual CGameObject* searchObjectInScene(const wchar_t* objectName);

		virtual CGameObject* searchObjectInChildByTemplateObjId(const char* id);

		virtual bool testConflictName(const wchar_t* objectName);

		std::string generateObjectName(const char* objTemplate);

		void registerObjectInSearchList(CGameObject* obj);

		void removeObject(CGameObject* pObj);

		void addChild(CGameObject* p);

		inline ArrayGameObject* getChilds()
		{
			return &m_childs;
		}

		void removeAllObject(bool force = false);

		template<typename T>
		void getListObjectType(ArrayGameObject& listObjs, T type);

		template<typename T>
		std::vector<T*> getComponentsInChild(bool addThis);

		core::array<CGameObject*>& getArrayChilds(bool addThis);

		bool haveChild(CGameObject* gameObject);

		DECLARE_GETTYPENAME(CContainerObject)
	};

	template<typename T>
	std::vector<T*> CContainerObject::getComponentsInChild(bool addThis)
	{
		std::vector<T*> result;
		std::queue<CGameObject*> queueObjs;

		if (addThis == true)
			queueObjs.push(this);
		else
		{
			for (CGameObject*& obj : m_childs)
				queueObjs.push(obj);
		}

		while (queueObjs.size() != 0)
		{
			CGameObject* obj = queueObjs.front();
			queueObjs.pop();

			T* comp = obj->getComponent<T>();
			if (comp != NULL)
				result.push_back(comp);

			CContainerObject* container = dynamic_cast<CContainerObject*>(obj);
			if (container != NULL)
			{
				for (CGameObject*& child : container->m_childs)
					queueObjs.push(child);
			}
		}

		return result;
	}
}