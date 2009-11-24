/* Generated by Together */

#ifndef GOTOTASK_H
#define GOTOTASK_H

#include "AbstractTask.hpp"
class Waypoint;

/**
 * Class providing ability to go to a single task point
 */
class GotoTask : public AbstractTask {
public:
  /** 
   * Base constructor.
   * 
   * @param te Task events callback class (shared among all tasks) 
   * @param tb Global task behaviour settings
   * @param ta Advance mechanism used for advancable tasks
   * @param gp Global glide polar used for navigation calculations
   * 
   * @return Initialised object (with no waypoint to go to)
   */

  GotoTask(const TaskEvents &te, 
           const TaskBehaviour &tb,
           TaskAdvance &ta,
           GlidePolar &gp);
  ~GotoTask();

/** 
 * Size of task
 * 
 * @return Number of taskpoints in task
 */
  virtual unsigned task_size() const;

/** 
 * Retrieves the active task point sequence.
 * 
 * @return Index of active task point sequence
 */
  virtual TaskPoint* getActiveTaskPoint() const;

/** 
 * Set active task point index
 * 
 * @param index Desired active index of task sequence
 */
  virtual void setActiveTaskPoint(unsigned index);

/** 
 * Sets go to task point to specified waypoint. 
 * 
 * @param wp Waypoint to Go To
 */
  void do_goto(const Waypoint& wp);

/** 
 * Update internal states when aircraft state advances.
 * 
 * @param state_now Aircraft state at this time step
 * @param full_update Force update due to task state change
 *
 * @return True if internal state changes
 */
  virtual bool update_sample(const AIRCRAFT_STATE &state_now, 
                             const bool full_update);
protected:
/** 
 * Test whether (and how) transitioning into/out of task points should occur, typically
 * according to task_advance mechanism.  This also may call the task_event callbacks.
 * 
 * @param state_now Aircraft state at this time step
 * @param state_last Aircraft state at previous time step
 * 
 * @return True if transition occurred
 */
  virtual bool check_transitions(const AIRCRAFT_STATE& state_now, 
                                 const AIRCRAFT_STATE& state_last);

private:    
  TaskPoint* tp;
public:
#ifdef DO_PRINT
  virtual void print(const AIRCRAFT_STATE &state);
#endif

/** 
 * Accept a task point visitor; makes the visitor visit
 * all TaskPoint in the task
 * 
 * @param visitor Visitor to accept
 */
  void Accept(TaskPointVisitor& visitor) const;
  DEFINE_VISITABLE()
};

#endif //GOTOTASK_H
