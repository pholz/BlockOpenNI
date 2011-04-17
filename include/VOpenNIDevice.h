#pragma once

#include "VOpenNICommon.h"
#include "VOpenNISurface.h"
#include "VOpenNIUser.h"


namespace V
{

	// Forward declarations
	class OpenNIUser;
	class OpenNIDevice;
	class UserListener;

	// Typedefs
	typedef boost::shared_ptr<OpenNIUser> OpenNIUserRef;
	typedef boost::shared_ptr<OpenNIDevice> OpenNIDeviceRef;
	typedef std::list< boost::shared_ptr<OpenNIDevice> > OpenNIDeviceList;
	typedef std::list< OpenNIUserRef > OpenNIUserList;
	typedef std::vector<UserListener*> UserListenerList;






	struct UserEvent
	{
		UserEvent()
		{
			mId = 0;
			mDevice = OpenNIDeviceRef();
			mUser = OpenNIUserRef();
		}
		uint32_t mId;
		OpenNIDeviceRef mDevice;
		OpenNIUserRef	mUser;
	};

	class UserListener
	{
	public:
		virtual void onNewUser( UserEvent event ) {};
		virtual void onLostUser( UserEvent event ) {};
	};


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	class OpenNIDevice
	{
	public:
		typedef boost::shared_ptr<OpenNIDevice> Ref;

	public:
		OpenNIDevice( xn::Context* context );
		~OpenNIDevice();
		bool init( boost::uint64_t nodeTypeFlags );
		bool initFromXmlFile( const std::string& xmlFile, bool allocUserIfNoNode=false );

		void release();

		void start();

		void allocate( uint64_t flags, uint32_t width, uint32_t height );

		bool requestUserCalibration();
		void setResolution( ProductionNodeType nodeType, int res, int fps );
		void setFPS( ProductionNodeType nodeType, int fps );
		void setMapOutputMode( ProductionNodeType nodeType, int width, int height, int fps );
		void readFrame();
		void calculateHistogram();

		void setLimits( int nearClip, int farClip );

		void setPrimaryBuffer( int type );

		// Shifts depth pixel (bit operator) NOTE!this fucks with the correct distance values
		// To get correct distances, set this value to 0 (zero)
		int getDepthShiftMul()						{ return mDepthShiftValue; }
		void setDepthShiftMul( int value )			{ mDepthShiftValue = value; }
		void resetDepthShiftMul()					{ mDepthShiftValue = 0; }

		void setDepthInvert( bool flag );
		bool getDepthInvert()						{ return _isDepthInverted; }

		boost::uint8_t* getColorMap();
		boost::uint16_t* getIRMap();
		boost::uint8_t* getIRMap8i();
		boost::uint16_t* getDepthMap();
		//boost::uint8_t* getDepthMap8i();
		boost::uint8_t* getDepthMap24();
		boost::uint16_t* getRawDepthMap();

		xn::DepthMetaData* getDepthMetaData()		{ return _depthMetaData; }
		xn::SceneMetaData* getUserMetaData()		{ return _sceneMetaData; }
		xn::ImageGenerator*	getImageGenerator()		{ return &_imageGen;	}
		xn::IRGenerator* getIRGenerator()			{ return &_irGen;	}
		xn::DepthGenerator*	getDepthGenerator()		{ return &_depthGen;	}
		xn::UserGenerator* getUserGenerator()		{ return &_userGen;	}
		xn::HandsGenerator* getHandsGenerator()		{ return &_handsGen;	}
		xn::Context*	getContext()				{ return _context;	}

		bool isOneTimeCalibration()					{ return _isOneTimeCalibration;	}
		void enableOneTimeCalibration( bool value )	{ _isOneTimeCalibration = value;	}
		void enableFileCalibration( bool value )	{ _isFileCalibration = value;	}
		bool isFileCalibration()					{ return _isFileCalibration; }

		// Set calibration state. true/false
		void setCalibrationState( bool value )		{ _isFirstCalibrationComplete = value;	}

		float getMinDistance()						{ return mMinDistance; }
		float getMaxDistance()						{ return mMaxDistance; }

		void setAlignWithDepthGenerator();

		//const std::string& getDebugInfo()			{ return mDebugInfo;}

		void addUser( uint32_t id );
		OpenNIUserRef getUser( uint32_t id );
		void removeUser( uint32_t id );
		bool hasUser( int32_t id );
		bool hasUsers()								{ return (mUserList.size()>0)?true:false; }


		XnSkeletonProfile getSkeletonProfile()	{ return mSkeletonProfile;	}
		void setSkeletonProfile( XnSkeletonProfile profile ) 
		{ 
			mSkeletonProfile = profile; 
			if( _isUserOn && _userGen.IsCapabilitySupported(XN_CAPABILITY_SKELETON) )
			{
				_userGen.GetSkeletonCap().SetSkeletonProfile( mSkeletonProfile );
			}
		}


		uint32_t addListener( UserListener* listener )
		{
			mListeners.push_back( listener );
			return mListeners.size()-1;
		}
		void removeListener( uint32_t id )
		{
			//mListeners.erase( id );
		}
		UserListenerList getListeners()			{ return mListeners;	}

		static void XN_CALLBACK_TYPE Callback_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie );
		static void XN_CALLBACK_TYPE Callback_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie );
		static void XN_CALLBACK_TYPE Callback_PoseDetected( xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie );
		static void XN_CALLBACK_TYPE Callback_PoseDetectionEnd( xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie );
		static void XN_CALLBACK_TYPE Callback_CalibrationStart( xn::SkeletonCapability& capability, XnUserID nId, void* pCookie );
		static void XN_CALLBACK_TYPE Callback_CalibrationEnd( xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie );


	private:
		void run();
	public:
		static const bool		USE_THREAD;
		boost::shared_ptr<boost::thread> _thread;

		std::string				mDeviceName;
		//std::string				mDebugInfo;

		bool					_isRunning;
		bool					_isDepthInverted;

		//std::string				_configFile;

		xn::Context*			_context;	// Pointer to context in device manager

		xn::Device*				_device;	// Device object

		xn::EnumerationErrors	_errors;
		XnStatus				_status;

		//XnFPSData				_fpsData;
		XnMapOutputMode			_mapMode; 


		UserListenerList		mListeners;

		// Buffers
		OpenNISurface8*			mColorSurface;
		OpenNISurface8*			mIRSurface;
		OpenNISurface16*		mDepthSurface;

		const int				mBitsPerPixel;
		//boost::uint8_t*			_colorData;
		boost::uint16_t*		_irData;
		boost::uint8_t*			_irData8;
		boost::uint16_t*		_depthData;
		boost::uint16_t*		_backDepthData;
		//boost::uint8_t*			_depthData8;
		boost::uint8_t*			_depthDataRGB;

		XnRGB24Pixel*			g_pTexMap;
		int						g_MaxDepth;
		float*					g_pDepthHist;

		int						mDepthShiftValue;	// pixel shift left value (intensifies the distance map)
		float					mMinDistance, mMaxDistance;

		XnSkeletonProfile		mSkeletonProfile;

		// Temp buffers
		const XnUInt8*			pImage;
		const XnDepthPixel*		pDepth;
		const XnIRPixel*		pIR;

		// Flags for nodes
		bool					_isImageOn;
		bool					_isIROn;
		bool					_isDepthOn;
		bool					_isUserOn;
		bool					_isAudioOn;
		bool					_isHandsOn;

		// User Calibrations
		bool					_isOneTimeCalibration;
		bool					_isFirstCalibrationComplete;
		bool					_isFileCalibration;

		xn::Generator*			_primaryGen;
		xn::DepthGenerator		_depthGen;
		xn::ImageGenerator		_imageGen;
		xn::IRGenerator			_irGen;
		xn::UserGenerator		_userGen;
		xn::AudioGenerator		_audioGen;
		xn::HandsGenerator		_handsGen;

		xn::ImageMetaData*		_imageMetaData;
		xn::IRMetaData*			_irMetaData;
		xn::DepthMetaData*		_depthMetaData;
		xn::SceneMetaData*		_sceneMetaData;
		xn::AudioMetaData*		_audioMetaData;


		// Internal clip planes
		int						mNearClipPlane, mFarClipPlane;

		// Users
		OpenNIUserList			mUserList;
	};





	/************************************************************************/
	/* Device Manager
	*/
	/************************************************************************/

	// A singleton
	class OpenNIDeviceManager : private boost::noncopyable
	{
	public:
		OpenNIDeviceManager();
		~OpenNIDeviceManager();

		uint32_t enumDevices();
		OpenNIDeviceRef createDevice( const std::string& xmlFile, bool allocUserIfNoNode=false );
		OpenNIDeviceRef createDevice( int nodeTypeFlags );
		//OpenNIDevice* createDevice__( const std::string& xmlFile, bool allocUserIfNoNode=false );
		//OpenNIDevice* createDevice__( int nodeTypeFlags );
		//void destroyDevice( OpenNIDevice* device );
		void destroyAll( void );

		OpenNIUserRef addUser( xn::UserGenerator* userGen, uint32_t id );
		void removeUser( uint32_t id );
		OpenNIUserRef getFirstUser();
		OpenNIUserRef getSecondUser();
		OpenNIUserRef getLastUser();
		OpenNIUserRef getUser( int id );
		bool hasUser( int32_t id );
		bool hasUsers();
		const uint32_t getNumOfUsers();
		OpenNIUserList getUserList();

		void start();

		void renderJoints( float width, float height, float depth, float pointSize, bool renderDepth=false );

		const std::string& getDebugInfo()			{ return mDebugInfo;}
		void setText( const std::string& info )		{ mDebugInfo = info; }

		const uint32_t getMaxNumOfUsers()			{ return mMaxNumOfUsers; }
		void setMaxNumOfUsers( uint32_t count )		{ mMaxNumOfUsers = count; }

		//
		// Instance
		//
		static OpenNIDeviceManager& Instance() 
		{
			return _singletonPointer;
		}
		static OpenNIDeviceManager* InstancePtr() 
		{
			return &_singletonPointer;
		}

		void update();

	private:
		// Copy constructor
		OpenNIDeviceManager( const OpenNIDeviceManager& ) {};
		// Operators
		OpenNIDeviceManager& operator = ( const OpenNIDeviceManager& ) {};

		void run();
	public:
		static const bool				USE_THREAD;


	protected:
		static OpenNIDeviceManager		_singletonPointer;

		boost::shared_ptr<boost::thread> _thread;
		boost::mutex					 _mutex;
		boost::recursive_mutex			 _rmutex;
		bool							_isRunning;

		xn::Context						_context;

		std::string						mDebugInfo;

		uint32_t						mMaxNumOfUsers;

		int								_idCount;

		// Device list
		OpenNIDeviceList				mDevices;

		// Generic user list. These users have no knowledge of which device they come from
		OpenNIUserList					mUserList;
	};

}	// V