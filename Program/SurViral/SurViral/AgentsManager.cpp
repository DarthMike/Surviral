/*
	Filename: AgentsManager.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to create and manage agents in-game
	Comments: A game agent is anything that reacts to game logic: Physic box, enemy, triggerpoint, coins, bombs...
			  This class calls update methods for created agents in run-time, and acts as "factory" to agents
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AgentsManager.h"
#include "AgentsManagerListener.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include <sstream>

//Static variables definition
int AgentsManager::mAgentCount = 0;

//Create a new agent instance
IAgent* AgentsManager::CreateNewAgent(const std::string& name,const GameAgentPar *newagentparams )
{
	//Pointer to return
	IAgent* newagent = NULL;
	bool agentcreated(false);
	//Check validity of name
	GameAgentsMapIterator itr = mAgentsMap.find(name);
	if( itr != mAgentsMap.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("AgentsManager::CreateNewAgent","ERROR, attempt to create agent with same name twice",LOGEXCEPTION);
		return NULL;
	}

	switch(newagentparams->GetType())
	{
	case(PHYSICBODY):
		{
			newagent = new SolidBodyAgent(name,this,mPhysicsManager);
			newagent->Create(newagentparams);
			agentcreated = true;
		}	
		break;
	case (AI):
		{
			newagent = new AIAgent(name,this,mPhysicsManager);
			newagent->Create(newagentparams);
			agentcreated = true;
		}	
		break;
	case(PLAYER):
		{
			newagent = new PlayerAgent(name,this,mPhysicsManager);
			newagent->Create(newagentparams);
			agentcreated = true;
		}
		break;
	case(PROJECTILE):
		{
			newagent = new ProjectileAgent(name,this,mPhysicsManager);
			newagent->Create(newagentparams);
			agentcreated = true;
		}
		break;
	default: //UNDEFINED
		{
			SingletonLogMgr::Instance()->AddNewLine("AgentsManager::CreateNewAgent","ERROR, attempt to create undefined type of agent",LOGEXCEPTION);
		}
		break;
	}

	//IF -  Agent created
	if(agentcreated)
	{
		//Update number of created agents
		mAgentsMap[name] = newagent;
		++mAgentCount;

		//Notify other elements in game of new agent

		//NOTE: I DONT LIKE IT!!! BREAKS POLYMORPHISM HERE...
		std::string aitype;
		if(newagent->GetType() == AI)
		{
			AIAgent* aiagent (static_cast<AIAgent*>(newagent));
			aitype = aiagent->GetAIType();
		}

		NewAgentCreatedInfo eventdata (name,newagent->GetType(),newagent->GetPosition(),aitype);
		SingletonGameEventMgr::Instance()->TriggerEvent(
										EventDataPointer(new NewAgentCreatedEvent(Event_NewAgentCreated,eventdata))
										);
	}//IF

	return(newagent);
}

//Update all available agents state
void AgentsManager::UpdateAgents(float dt)
{
	//Iterate through map and update elements
	GameAgentsMapIterator itr = mAgentsMap.begin();
	//LOOP - Update all created agents
	while(itr != mAgentsMap.end())
	{
		//IF - AGENT IS ALIVE
		if((*itr).second->IsAlive())
		{
			//Update selected element
			(*itr).second->UpdateState(dt);
			++itr;
		}
		else //ELSE - AGENT WAS DESTROYED
		{
			//Send event of agent deletion 
			//Event_AgentDestroyed			
			//Notify other elements in game of new agent
			//NOTE: I DONT LIKE IT!!! BREAKS POLYMORPHISM HERE...
			std::string aitype;
			if((*itr).second->GetType() == AI)
			{
				AIAgent* aiagent (static_cast<AIAgent*>((*itr).second));
				aitype = aiagent->GetAIType();
			}
			AgentDestroyedInfo eventdata ((*itr).first,(*itr).second->GetType(),(*itr).second->GetPosition(),aitype);
		
			SingletonGameEventMgr::Instance()->TriggerEvent(
										EventDataPointer(new AgentDestroyedEvent(Event_AgentDestroyed,eventdata))
										);
			delete (*itr).second;
			itr = mAgentsMap.erase(itr);
		}
	}//LOOP END
}

//Search for a concrete agent
IAgent* AgentsManager::_searchAgent(const std::string& name)
{
	//Return NULL if not found
	GameAgentsMapIterator itr = mAgentsMap.find(name); 
	if(itr != mAgentsMap.end())
	{
		return((*itr).second);
	}
	
	//Not found
	return(NULL);	
}

//Handle collisions
bool AgentsManager::_handleEvents(const EventData& eventdata)
{
	bool eventprocessed(false);
	//Check received events are of correct type
	//IF - Collision events!
	if(eventdata.GetEventType() == Event_NewCollision
		||
		eventdata.GetEventType() == Event_PersistantCollision
		||
		eventdata.GetEventType() == Event_DeletedCollision
		||
		eventdata.GetEventType() == Event_CollisionResult
		)
	{
		const CollisionEventData& collevent (static_cast<const CollisionEventData&>(eventdata));
		
		const ContactInfo& theinfo (collevent.GetCollisionData());

		b2Body* body1 (theinfo.collidedbody1);
		if(body1)
		{
			//Forward event to this agent (collisions)
			if(theinfo.agent1)
			{
				//Check agent exists
				if(_agentExists(theinfo.agent1))
				{
					collevent.SetActiveBody(body1); //Memorize this is the body
					theinfo.agent1->HandleCollision(collevent);
				}
				else //DEBUG
				{
					#ifdef _DEBUGGING
					SingletonLogMgr::Instance()->AddNewLine("AGENTSMGR","Not forwared collision event",LOGDEBUG);
					std::stringstream ss;
					ss<<"NOT FORWARDED EVENT COLLISION!";
					DebugStringInfo themessage(ss.str());
					SingletonGameEventMgr::Instance()->QueueEvent(
													EventDataPointer(new DebugMessageEvent(Event_DebugString,themessage))
													);
					#endif
				}
			}
		}
		b2Body* body2 (theinfo.collidedbody2);
		if(body2)
		{
			//Forward event to this agent (collisions)
			if(theinfo.agent2)
			{
				//Check agent exists
				if(_agentExists(theinfo.agent2))
				{
					collevent.SetActiveBody(body2); //Memorize this is the body
					theinfo.agent2->HandleCollision(collevent);
				}	
				else //DEBUG
				{
					#ifdef _DEBUGGING
					SingletonLogMgr::Instance()->AddNewLine("AGENTSMGR","Not forwared collision event",LOGDEBUG);
					std::stringstream ss;
					ss<<"NOT FORWARDED EVENT COLLISION!";
					DebugStringInfo themessage(ss.str());
					SingletonGameEventMgr::Instance()->QueueEvent(
													EventDataPointer(new DebugMessageEvent(Event_DebugString,themessage))
													);
					#endif
				}
			}
		}
		eventprocessed = true;
	}//ELSE - Create new agent event
	else if(eventdata.GetEventType() == Event_CreateNewAgent)
	{
		const CreateNewAgentEvent& newagentevt (static_cast<const CreateNewAgentEvent&>(eventdata));
		const CreateNewAgentInfo& data (newagentevt.GetAgentInfo());

		//Create a new id and create agent
		std::string createid(data.agentid);
		if(createid == "")
		{
			std::stringstream newid;
			newid<<"_DYNAMICAGENT_"<<mAgentCount;
			createid = newid.str();
		}
		CreateNewAgent(createid,data.params.get());	
	}//ELSE - Out of limits event
	else if(eventdata.GetEventType() == Event_OutOfLimits)
	{
		const OutOfLimitsEventData& oolevent (static_cast<const OutOfLimitsEventData&>(eventdata));
		const OutOfLimitsData& data = oolevent.GetEventData();
		
		//Handle event - Call "Destroy" for agent
		IAgent* agent = data.agent;
		if(agent)
		{
			agent->HandleOutOfLimits(oolevent);
		}
		else
		{
			#ifdef _DEBUGGING
			std::stringstream ss;
			ss<<"EVENT OOL FOR: "<<agent->GetId()<<" WITHOUT AGENT!!!";
			DebugStringInfo themessage(ss.str());
			SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new DebugMessageEvent(Event_DebugString,themessage))
											);
			#endif
		}
		eventprocessed = true;
	}//ELSE - Joint destroyed event
	else if(eventdata.GetEventType() == Event_DestroyedJoint)
	{
		//Event is for all agents
		GameAgentsMapIterator itr;	
		//LOOP - Call all agents to receive this event
		for(itr = mAgentsMap.begin();itr != mAgentsMap.end(); ++itr)
		{
			//Forward event to this agent (unconverted data)
			(*itr).second->HandleEvent(eventdata);
		}//LOOP END
		eventprocessed = true;
	}//ELSE - Render in layer event
	else if(eventdata.GetEventType() == Event_RenderInLayer)
	{
		//Event is for all agents
		GameAgentsMapIterator itr;	
		//LOOP - Call all agents to receive this event
		for(itr = mAgentsMap.begin();itr != mAgentsMap.end(); ++itr)
		{
			//Forward event to this agent (unconverted data)
			(*itr).second->HandleEvent(eventdata);
		}//LOOP END
		eventprocessed = true;
	}//ELSE - Other Events for player!
	else if(eventdata.GetEventType() == Event_MoveCommand
		    ||
			eventdata.GetEventType() == Event_ShootCommand
			||
			eventdata.GetEventType() == Event_DataBlockInfected
			||
			eventdata.GetEventType() == Event_NewAgentCreated
			||
			eventdata.GetEventType() == Event_AntiVirusNewTargetRequest
			||
			eventdata.GetEventType() == Event_VirusTerminated)
	{
		GameAgentsMapIterator itr;	
		//LOOP - Call all agents to receive this event
		for(itr = mAgentsMap.begin();itr != mAgentsMap.end(); ++itr)
		{
			//IF - Is the player agent
			if((*itr).second->GetType() == PLAYER)
			{
				//Forward event to this agent (unconverted data)
				eventprocessed = (*itr).second->HandleEvent(eventdata);
			}
		}//LOOP END
	}//ELSE - Incoherent type
	else
	{
		//Never!!!
		assert(false);
	}

	return eventprocessed;
}

//Query if an agent exists
bool AgentsManager::_agentExists(IAgent* agenttosearch)
{
	bool agentfound(false);
	GameAgentsMapIterator itr;	
	//LOOP - Search for agent with this pointer
	for(itr = mAgentsMap.begin();itr != mAgentsMap.end(); ++itr)
	{
		//IF - Is the player agent
		if((*itr).second == agenttosearch)
		{
			agentfound = true;
			break;
		}
	}//LOOP END	

	return agentfound;
}

void AgentsManager::_init()
{
	mEventListener = new AgentsManagerListener(this);
}

void AgentsManager::_release()
{
	//Delete al dynamically created agents
	GameAgentsMapIterator itr;	
	
	//LOOP - Delete all created agents
	for(itr = mAgentsMap.begin();itr != mAgentsMap.end(); ++itr)
	{
		if((*itr).second)
			delete (*itr).second;
	}//LOOP END
	mAgentsMap.clear();

	if(mEventListener)
	{
		delete mEventListener;
		mEventListener = NULL;
	}
}