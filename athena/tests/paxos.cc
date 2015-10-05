#include <elle/test.hh>

#include <reactor/scheduler.hh>

#include <athena/paxos/Paxos.hh>

ELLE_LOG_COMPONENT("athena.paxos.test");

namespace paxos = athena::paxos;

template <typename T, typename ServerId>
class Peer
  : public paxos::Paxos<T, ServerId>::Peer
{
  virtual
  boost::optional<std::pair<int, T>>
  propose(ServerId const& sender, int round) override
  {
    return this->_paxos->propose(sender, round);
  }

  virtual
  void
  accept(ServerId const& sender, int round, T const& value) override
  {
    this->_paxos->accept(sender, round, value);
  }

  ELLE_ATTRIBUTE_RW((paxos::Paxos<T, ServerId>*), paxos);
};

ELLE_TEST_SCHEDULED(single)
{
  auto peer_1_2 = new Peer<int, int>();
  auto peer_1_3 = new Peer<int, int>();
  auto peer_2_1 = new Peer<int, int>();
  auto peer_2_3 = new Peer<int, int>();
  auto peer_3_1 = new Peer<int, int>();
  auto peer_3_2 = new Peer<int, int>();

  typedef paxos::Paxos<int, int>::Peers Peers;
  Peers peers_1;
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_2));
  peers_1.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_1_3));
  paxos::Paxos<int, int> paxos_1(1, std::move(peers_1));
  Peers peers_2;
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_1));
  peers_2.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_2_3));
  paxos::Paxos<int, int> paxos_2(2, std::move(peers_2));
  Peers peers_3;
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_1));
  peers_3.push_back(std::unique_ptr<paxos::Paxos<int, int>::Peer>(peer_3_2));
  paxos::Paxos<int, int> paxos_3(3, std::move(peers_3));

  peer_1_2->paxos(&paxos_2);
  peer_1_3->paxos(&paxos_3);
  peer_2_1->paxos(&paxos_1);
  peer_2_3->paxos(&paxos_3);
  peer_3_1->paxos(&paxos_1);
  peer_3_2->paxos(&paxos_2);

  paxos_1.choose(42);
  BOOST_CHECK_EQUAL(paxos_2.choose(), 42);
  BOOST_CHECK_EQUAL(paxos_3.choose(), 42);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(single), 0, valgrind(1));
}
