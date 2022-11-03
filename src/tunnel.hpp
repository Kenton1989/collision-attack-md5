#ifndef COLLISION_ATTACK_TUNNEL_HPP_
#define COLLISION_ATTACK_TUNNEL_HPP_

#include <stddef.h>

#include <string>
#include <vector>
#include "md5.hpp"
#include "constants.hpp"

namespace Tunnel{


class Tunnel{
    public:
        Tunnel(const int start_step, const int tunneled_step, const int * involved_message_words);
        Md5::Words Evaluate();
        

    private: 
};



}


#endif