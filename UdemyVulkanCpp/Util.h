//
// Created by Marcel Vilalta i Soler on 21/4/24.
//

#ifndef UDEMYVULKANCPP_UTIL_H
#define UDEMYVULKANCPP_UTIL_H



// Indices (locations) of Queues families if they exist at all

struct QueueFamilyIndices {
    int graphicsFamily = -1; // Location of the Graphic family

    bool isValid() {
        return graphicsFamily >= 0;
    }


};

#endif //UDEMYVULKANCPP_UTIL_H
