#ifndef _GY_JOBMGR_H_
#define _GY_JOBMGR_H_

class gyEngine;
class gyJob : public gyRefCounted
{
public:
  virtual int Execute( ) = 0;
};

struct gyJobInfo : public gyResourceBase
{
  gySharedPtr<gyJob> job;
  gyIDJob jobId;
  gyIDJob parentId;
  gyIDJob depId;
  uint16_t priority;
  uint16_t jobFlags;
  uint16_t depCount;
  uint16_t scheduled;
};

class gyJobManager : public gyReflectedObject
{
  GY_DECLARE_REFLECT(gyJobManager);
public:
  gy_override int Create(gyVarDict& createParams);
  gy_override void FillCreateTemplate(gyVarDict& outTemplate);

  // -- Job API
  gyIDJob JobCreate( gySharedPtr<gyJob> job, const char* jobName=NULL, int priority=0 );
  void  JobRemove( gyIDJob jid );
  void  JobSchedule( gyIDJob jid );
  void  JobParent( gyIDJob jid, gyIDJob parentJid );
  void  JobDependency( gyIDJob jid, gyIDJob dependJid );
  void  JobKickoffSystem( bool resolveDeps=false );
  void  JobWaitSystem();
  uint16_t      JobIsScheduled( gyIDJob jid );
  const char* JobGetName( gyIDJob jid );

private:
  friend class gyEngine;
  ~gyJobManager();
  gyResourceMgr<gyJobInfo, ID_JOB>  jobs;
  
};

#endif