#ifndef BQ_CMD
#define BQ_CMD 
void addTopic();
void deleteTopic();
void getTopicInfo();
void addMessage(int topicId, int message);
void getMessage(int topicId);
void reuseMessage();
void sleepMessage();
void wakeupMessage();


#endif