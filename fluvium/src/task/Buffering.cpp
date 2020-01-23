#include "task/Buffering.h"

using namespace buffering;

template <typename DATA_TYPE> 
static jsonStruct_t create(char * nameKey, DATA_TYPE value, JsonPrimitiveType jsonType) {
    jsonStruct_t data { nameKey, &value, sizeof(value), jsonType, NULL };
    return data;
}

//BUFFER IMPLEMENTATION
Buffer::Buffer(int size) : xQueue(xQueueCreate(size, sizeof(Data *))) {};

bool Buffer::queue(const Data* data) const {
    return xQueueSend(xQueue, &data, portMAX_DELAY) == pdTRUE;
}

void Buffer::clear() const {
    xQueueReset(xQueue);    
}

Data* Buffer::dequeue() const {
    Data* data;
    xQueueReceive(xQueue, &data, portMAX_DELAY);
    return data;
}

bool Buffer::isEmpty() const {
    Data* data = nullptr;
    xQueuePeek(xQueue, &data, 0);
    return data == nullptr; 
}
//BASE PARSER IMPLEMENTATION

#define MAX_FINALIZATION_LEN 50
Parser::Parser(data_type parsingType) : parsingType(parsingType) {};

json Parser::serialize(const Data& data) {
    if(data.id == parsingType) {
        return doSerialize(data);
    } else {
        return "";
    }
}