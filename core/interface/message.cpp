#include "engine.h"

namespace ink {

Ink_Object *Ink_InterpreteEngine::receiveMessage()
{
	Ink_Object *ret = NULL;
	Ink_ActorMessage *msg = NULL;
	Ink_InterpreteEngine *engine = this;

	pthread_mutex_lock(&message_lock);
	if (!message_queue.empty()) {
		msg = message_queue.front();
		ret = new Ink_Object(this);
		ret->setSlot_c("msg", new Ink_String(this, *(msg->msg)));
		ret->setSlot_c("sender", new Ink_String(this, *(msg->sender)));
		ret->setSlot_c("ex", msg->ex
							 ? (Ink_Object *)msg->ex->toObject(this)
							 : (Ink_Object *)UNDEFINED);
		delete msg;
		message_queue.pop();
	}
	pthread_mutex_unlock(&message_lock);
	return ret;
}

void Ink_InterpreteEngine::sendInMessage(Ink_InterpreteEngine *sender, string msg, Ink_ExceptionRaw *ex)
{
	pthread_mutex_lock(&message_lock);
	message_queue.push(new Ink_ActorMessage(new string(msg), InkActor_getActorName(sender), ex));
	pthread_mutex_unlock(&message_lock);
	return;
}

void Ink_InterpreteEngine::sendInMessage_nolock(Ink_InterpreteEngine *sender, string msg, Ink_ExceptionRaw *ex)
{
	pthread_mutex_lock(&message_lock);
	message_queue.push(new Ink_ActorMessage(new string(msg), InkActor_getActorName_nolock(sender), ex));
	pthread_mutex_unlock(&message_lock);
	return;
}

void Ink_InterpreteEngine::disposeAllMessage()
{
	pthread_mutex_lock(&message_lock);
	while (!message_queue.empty()) {
		delete message_queue.front();
		message_queue.pop();
	}
	pthread_mutex_unlock(&message_lock);
	return;
}

void Ink_InterpreteEngine::addWatcher(string name)
{
	pthread_mutex_lock(&watcher_lock);
	watcher_list.push_back(name);
	pthread_mutex_unlock(&watcher_lock);
	return;
}

void Ink_InterpreteEngine::broadcastWatcher(string msg, Ink_ExceptionRaw *ex)
{
	Ink_InterpreteEngine *tmp_engine = NULL;
	Ink_ActorWatcherList::iterator w_iter;

	for (w_iter = watcher_list.begin();
		 w_iter != watcher_list.end(); w_iter++) {
		InkActor_lockActorLock();
		if ((tmp_engine = InkActor_getActor_nolock(*w_iter)) != NULL) {
			tmp_engine->sendInMessage_nolock(this, msg, ex);
		}
		InkActor_unlockActorLock();
	}

	return;
}

}
