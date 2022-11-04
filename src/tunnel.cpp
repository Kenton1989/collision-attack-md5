#include "tunnel.hpp"

#include <string>
#include <vector>
#include "md5.hpp"
#include "constants.hpp"


namespace Tunnel {
using std::vector;
using Words = Md5::Words;
using Word = Md5::Word;
using Byte = Md5::Byte;
using Bytes = Md5::Bytes;

Tunnel::Tunnel(const int start_step, 
    const Words start_step_iv,
    const int tunneled_step, 
    const vector<int> involved_message_idxs, 
    const vector<int> free_bit_positions)
    {
        this->set_start_step_iv(start_step_iv);
        this->_tunneled_step = tunneled_step;
        this->_involved_message_idxs = involved_message_idxs;
        this->_free_bit_positions = free_bit_positions;

        
    }

vector<Words> Tunnel::generate_other_povs()
{

}

Words Tunnel::get_start_step_iv()
{

}

void Tunnel::set_start_step_iv(Words value)
{

}

int Tunnel::get_tunneled_step()
{

}

Word Tunnel::solve_message(const int step)
{
    
}

Word Tunnel::solve_Q(const int step)
{

}

}