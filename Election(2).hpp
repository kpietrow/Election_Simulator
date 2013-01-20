// Election.hpp
// Kevin Pietrow


#ifndef ELECTION_HPP_
#define ELECTION_HPP_

#include <random>
#include <bitset>
#include <iostream>
#include <memory>
#include <initializer_list>
#include <ctime>

using namespace std;

using Ballot = std::bitset<3>;

enum Party {DEMOCRAT, REPUBLICAN, INDEPENDENT};

class Voter	{
public:
	// virtual, abstract methods for Base and Swing voters
	virtual bool vote(Ballot & blt) = 0;
	virtual Party affiliation() const = 0;
};

using Electorate = std::vector<std::unique_ptr<Voter>>;

template <Party P>
class BaseVoter : public Voter 
{
public:
	BaseVoter(double enthusiasm)
		: motivated(enthusiasm) {}
		
	// vote() method
	bool vote(Ballot & blt);
	
	// return Party
	Party affiliation() const { return P; } ;
private:
	// <random> methods
	std::bernoulli_distribution motivated;

};


// and in a galaxy far far away...
/*
template <Party P>
BaseVoter<P>::BaseVoter( double enthusiasm )
{

}
*/

template <Party P>
class SwingVoter : public Voter
{
public:
	// SwingVoter ctr and template
	template <typename InputIt>
	SwingVoter(double enthusiasm, InputIt first, InputIt last)
		: motivated(enthusiasm), choice(first, last) {} 
		
	bool vote(Ballot & blt);
	Party affiliation() const { return P; };
	
private:
	// <random> methods
	std::bernoulli_distribution motivated; 
	std::discrete_distribution<unsigned> choice;
};

/* private:
	unsigned m_popnsize;
	// std::discrete_distribution<unsigned> m_chooseparty;
	
	std::bernoulli_distribution dem_undecided
	std::bernoulli_distribution rep_undecided
	std::bernoulli_distribution ind_undecided
	
	std::vector<double> turnout;
	std::vector<std::vector<double>> weights;
	
	std::mt19937 rng;
};

Election::Election ( unsigned. ..........
					..........
					..........)
	: chooseparty( strength ), dem_undecided(*(undecided.begin())),
		rep_undecided(*(undecided.begin() + 1)), ...


*/

class Election	{ 
public:
	Election(unsigned popnsize, 
			std::initializer_list<double> strength, 
			std::initializer_list<double> undecided, 
			std::initializer_list<double> turnout, 
			std::initializer_list<std::initializer_list<double>> weights)
				: chooseparty( strength ), dem_undecided(*(undecided.begin())), rep_undecided(*(undecided.begin() + 1)), 
					ind_undecided(*(undecided.begin() + 2)), m_turnout(std::begin(turnout), std::end(turnout)), m_popnsize( popnsize ), 
					m_weights(std::begin(weights), std::end(weights)), generator(std::time(0)) {};
	void simulate() ;

private:
	std::discrete_distribution<unsigned> chooseparty;
	
	// for undecided
	std::bernoulli_distribution dem_undecided;					
	std::bernoulli_distribution rep_undecided;
	std::bernoulli_distribution ind_undecided;
	
	// percent that will turn out and total pop
	std::vector<double> m_turnout;
	unsigned m_popnsize;
	
	// weights for SwingVoters
	std::vector<std::vector<double>>  m_weights;
	
	// <random> methods
	std::random_device randev;
	std::default_random_engine generator;
	
	// counter vars
	int dem_voters = 0;
	int rep_voters = 0;
	int ind_voters = 0;
		// generate popnsize number of different voters
		// how to decide what kind of voter, how do we initialize voter?
		// maybe use discrete_distribution to determine which party
};

// vote() for BaseVoter
template <Party P>
bool BaseVoter<P>::vote(Ballot & blt)  
{
	std::default_random_engine generator(std::time(0));
	bool rdm = this->motivated(generator);
	cout << "motivated? " << rdm << endl;
	// check if motivated
	if (rdm)
	{
		// if motivated, vote along party lines
		blt.flip(this->affiliation());
	}
	
	return blt.any();
}

// vote() for SwingVoter
template <Party P>
bool SwingVoter<P>::vote(Ballot & blt)  
{
	std::default_random_engine generator(std::time(0));
	bool rdm = this->motivated(generator);
	cout << "motivated? " << rdm << endl;
	// check motivation
	if (rdm)
	{
		// calculate who they vote for
		blt.flip(this->choice(generator));
	}
	
	return blt.any();
}
		
void Election::simulate()
{
	Electorate voters;
	// for loop that instatiates voters
	for ( unsigned i = 0 ; i < this->m_popnsize; i++ )
	{
		auto party = chooseparty(generator);
		bool is_swing;
		switch (party)
		{
		case DEMOCRAT:
			is_swing = dem_undecided(generator);
			if (is_swing) {
				voters.push_back(std::unique_ptr<Voter>( new SwingVoter<DEMOCRAT> ( m_turnout[0], begin(m_weights[0]), end(m_weights[0]) )));
			// or std::unique_ptr<Voter> v { new SwingVoter<DEMOCRAT>( ... ) };
			// voters.push_back( v );
			// push_back is part of vector
			}	
			else {
				voters.push_back(std::unique_ptr<Voter>( new BaseVoter<DEMOCRAT>( m_turnout[0] )));
			// add a base voter instead
			}
			break;
		case REPUBLICAN:
			is_swing = rep_undecided(generator);
			if (is_swing) {
				voters.push_back(std::unique_ptr<Voter>( new SwingVoter<REPUBLICAN>(m_turnout[1], begin(m_weights[1]), end(m_weights[1]) )));
			}	
			else {
				voters.push_back(std::unique_ptr<Voter>( new BaseVoter<REPUBLICAN>(m_turnout[1] )));
			}
			break;
		default: /*INDEPENDENT*/
			is_swing = ind_undecided(generator);
			if (is_swing) {
				voters.push_back(std::unique_ptr<Voter>( new SwingVoter<INDEPENDENT>(m_turnout[2], begin(m_weights[2]), end(m_weights[2]) )));
			}	
			else {
				voters.push_back(std::unique_ptr<Voter>( new BaseVoter<INDEPENDENT>(m_turnout[2] )));
			}
		}
	}
	
	cout << "voter's size: " << voters.size() << endl;
	cout << "reps: " << this->rep_voters << endl;
	cout << "dems: " << this->dem_voters << endl;
	cout << "inds: " << this->ind_voters << endl;
	
	// casting and checking votes
	Ballot new_ballot;
	for ( unsigned i = 0 ; i < voters.size(); i++ )
	{
		if(voters[i]->vote(new_ballot)) 	// see if they voted
		{
			if ( new_ballot.test(0) == true )		// see who they voted for
			{
				this->dem_voters++;
			}	
		
			else if ( new_ballot.test(1) == true )
			{
				this->rep_voters++;
			}
		
			else
			{
				this->ind_voters++;
			}
		}
		
		new_ballot.reset();
		
	}
	
	// print results
	cout << "Number of Democratic voters: " << this->dem_voters << endl;
	cout << "Number of Republican voters: " << this->rep_voters << endl;
	cout << "Number of Independent voters: " << this->ind_voters << endl;
	
	
	// determine election winner
	if ( this->dem_voters > this->rep_voters && this->dem_voters > this->ind_voters)
	{
		cout << "The Democratic candidate wins!" << endl;
	}
	
	else if (this->rep_voters > this->dem_voters && this->rep_voters > this->ind_voters)
	{
		cout << "The Republican candidate wins!" << endl;
	}
	
	else
	{
		cout << "The Independent candidate wins!" << endl;
	}
	
}
	
#endif
