#include "measurement.h"

history_t* history;
time_t lastRecordTimestamp;

void cleanHistory() {
    if (history->first) {
        history_record_t* currentNode = history->first;
        while (currentNode) {
            history_record_t* oldNode = currentNode;
            currentNode = currentNode->next;
            delete oldNode;
        }
    }
    delete history;
}

history_record_t* appendToHistory(std::string measurementSerialized) {
    if (!history) {
        history = new history_t;
    }
    history_record_t* newNode = new history_record_t;
    newNode->measurement = measurementSerialized;

    if (history->last) {
        history->last->next = newNode;
        newNode->prev = history->last;
        history->last = newNode;
        history->length += 1;
        if (history->length > getPreferences()->history_length) {
            history_record_t* oldNode = history->first;
            history->first = oldNode->next;
            history->length -= 1;
            delete oldNode;
        }
    } else {
        history->first = newNode;
        history->last = newNode;
        history->length = 1;
    }

    return newNode;
}

history_record_t* periodicalAppendToHistory(time_t timestamp, std::string measurementSerialized) {
    preferences_t* preferences = getPreferences();
    if (!history || history->length < preferences->history_length || timestamp >= lastRecordTimestamp + preferences->history_records_period) {
        return appendToHistory(measurementSerialized);
    }
    return NULL;
}

history_t* getHistory() {
    return history;
}