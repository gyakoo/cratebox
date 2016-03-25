/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#pragma once
//
//
//
//
//
//
//
#define HK_CONFIG_SIMD 1
#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/System/Init/PlatformInit.cxx>
#include <Common/Base/System/Hardware/hkHardwareInfo.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Config/hkProductFeatures.h>
#include <Common/Base/Thread/Pool/hkThreadPool.h>
#include <Common/Base/Thread/TaskQueue/Default/hkDefaultTaskQueue.h>
#include <Common/Base/Thread/Pool/hkCpuThreadPool.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Visualize/hkVisualDebugger.h>
#include <Common/Visualize/hkProcessRegisterUtil.h>
#include <Common/Visualize/hkProcessFactory.h>
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/SceneData/Scene/hkxSceneUtils.h>
#include <Common/SceneData/SceneDataToGeometryConverter/hkxSceneDataToGeometryConverter.h>

#include <Common/Base/KeyCode.h>
#include <Common/Base/keycode.cxx>
#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_PHYSICS_2012
#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_FX
#include <Common/Base/Config/hkProductFeatures.cxx>

//
// This class performs hk base initialization
//
class Renderer
{
public:
	Renderer() 		
	{
	}

	hkResultEnum init()
	{
#		if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
		// Flush all denormal/subnormal numbers (2^-1074 to 2^-1022) to zero.
		// Typically operations on denormals are very slow, up to 100 times slower than normal numbers.
		_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#		endif

		// memory/system init
		{
			hkMemoryRouter* memRouter = hkMemoryInitUtil::initFreeListLargeBlock( 
				hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(8<<20) );
			if ( memRouter == HK_NULL )
				return HK_FAILURE;
			if ( hkBaseSystem::init( memRouter, errorReportFunction) != HK_SUCCESS )
				return HK_FAILURE;
		}		
		
		m_stepCounter = 0;

		// initialize visual debugger
		m_vdbConn.init();

		printf( "You must open Havok Visual Debugger to visualize the simulation. Default port.\n\n");	
		return HK_SUCCESS;
	}
	
	void quit()
	{
		m_vdbConn.quit();	
		hkBaseSystem::quit();
		hkMemoryInitUtil::quit();
	}

	void step(hkReal deltaTime)
	{
		HK_DISPLAY_STAR(hkVector4(0,0,0,0), 1.0f, hkColor::BLUE );
		HK_DISPLAY_STAR(hkVector4(10,0,0,0), 1.0f, hkColor::RED );

		HK_DISPLAY_TEXT("hello",hkColor::WHITE);

		static hkReal a = 0.0f; a+=deltaTime;
		hkTransform t;
		hkRotation r; r.setAxisAngle(hkVector4(0,0,1,0), HK_REAL_PI*0.25f);
		t.setRotation( r );
		t.setTranslation( hkVector4(0,0,0) );
		drawBox(t, hkVector4(0.5f,0.5f,0.5f), hkColor::CYAN );

		r.setAxisAngle(hkVector4(0,0,1,0), a);
		t.setRotation(r);
		drawSphere(t, 0.5f, hkColor::CYAN, 3);
		// stepping vdb
		m_vdbConn.step(1.0f/60.0f);
		
		++m_stepCounter;		
	}

	void drawBox(const hkTransform& objectToWorld, const hkVector4& halfExtents, const hkColor::Argb& color)
	{
		// points
		hkVector4 point[8];
		for (int i = 0; i < 8; ++i)
		{
			hkVector4 v = halfExtents;
			if ( i & 1 )	v( 0 ) *= -1.0f;
			if ( i & 2 )	v( 1 ) *= -1.0f;
			if ( i & 4 )	v( 2 ) *= -1.0f;
			point[i].setTransformedPos( objectToWorld, v );
		}

		// draw edges between points which differ in a single bit.
		for ( int i = 0; i < 8; ++i )
		{
			for ( int bit = 1; bit < 8; bit <<= 1 )
			{
				int j = i ^ bit;
				if ( i < j )
				{
					HK_DISPLAY_LINE( point[i], point[j], color );
				}
			}
		}
	}

	void drawSphere(const hkTransform& t,hkReal r,hkColor::Argb color,int depth)
	{
		drawArc(t,r,hkVector4(+1,0,0,0),hkVector4(0,+1,0,0),hkVector4(0,0,+1,0),depth,color);
		drawArc(t,r,hkVector4(-1,0,0,0),hkVector4(0,0,+1,0),hkVector4(0,+1,0,0),depth,color);
		drawArc(t,r,hkVector4(+1,0,0,0),hkVector4(0,0,+1,0),hkVector4(0,-1,0,0),depth,color);
		drawArc(t,r,hkVector4(-1,0,0,0),hkVector4(0,-1,0,0),hkVector4(0,0,+1,0),depth,color);
		drawArc(t,r,hkVector4(+1,0,0,0),hkVector4(0,0,-1,0),hkVector4(0,+1,0,0),depth,color);
		drawArc(t,r,hkVector4(-1,0,0,0),hkVector4(0,+1,0,0),hkVector4(0,0,-1,0),depth,color);
		drawArc(t,r,hkVector4(+1,0,0,0),hkVector4(0,-1,0,0),hkVector4(0,0,-1,0),depth,color);
		drawArc(t,r,hkVector4(-1,0,0,0),hkVector4(0,0,-1,0),hkVector4(0,-1,0,0),depth,color);
	}

	void drawArc(const hkTransform& t,hkReal r,const hkVector4& a,const hkVector4& b,const hkVector4& c,int depth,hkColor::Argb color)
	{
		hkVector4	ab,bc,ca;
		ab.setAdd4(a,b);ab.mul4(0.5f);ab.normalize3();
		bc.setAdd4(b,c);bc.mul4(0.5f);bc.normalize3();
		ca.setAdd4(c,a);ca.mul4(0.5f);ca.normalize3();
		if(depth>0)
		{		
			drawArc(t,r,a,ab,ca,depth-1,color);
			drawArc(t,r,b,bc,ab,depth-1,color);
			drawArc(t,r,c,ca,bc,depth-1,color);
			drawArc(t,r,ab,bc,ca,depth-1,color);
		}
		else
		{
			hkVector4	ct;
			ct.setAdd4(a,b);
			ct.add4(c);
			ct.mul4(1/3.0f);
			ct.normalize3();
			ct.mul4(r);ct.setTransformedPos(t,ct);
			ab.mul4(r);ab.setTransformedPos(t,ab);
			bc.mul4(r);bc.setTransformedPos(t,bc);
			ca.mul4(r);ca.setTransformedPos(t,ca);
			HK_DISPLAY_LINE(ct,ab,color);
			HK_DISPLAY_LINE(ct,bc,color);
			HK_DISPLAY_LINE(ct,ca,color);
		}
	}


private:
	static void HK_CALL errorReportFunction(const char* str, void*)
	{
		//printf( "***** %s\n", str); 
	}

private:
	//
	// This class in charge of connection with visual debugger
	//
	class VdbConnector
		{
		public:
			void init()
			{
				HK_ASSERT2( 0xf0f0f0f0, m_vdb == HK_NULL, "Vdb already setup" );
				m_vdbClassReg.setAndDontIncrementRefCount( new hkVtableClassRegistry );
				m_vdbClassReg->registerList( hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses );
				m_vdb.setAndDontIncrementRefCount( new hkVisualDebugger(m_contexts, m_vdbClassReg) );
				m_vdb->addDefaultProcess("Statistics");

				m_vdb->serve();
			}

			void quit()
			{
				m_contexts.clearAndDeallocate();
				m_vdb = HK_NULL;
				m_vdbClassReg = HK_NULL;
			}

			void step(hkReal deltaTime)
			{
				if ( m_vdb )
					m_vdb->step(deltaTime*1000.0f);
			}

		private:
			hkArray<hkProcessContext*> m_contexts;
			hkRefPtr<hkVisualDebugger> m_vdb;
			hkRefPtr<hkVtableClassRegistry> m_vdbClassReg;		
		};

private:
	hkUint32 m_stepCounter;
	VdbConnector m_vdbConn;
};

/*
 * Havok SDK
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
