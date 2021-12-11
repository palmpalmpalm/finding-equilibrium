import numpy as np

# b = bet/call, c = check/fold
ACTIONS = ['b', 'c']
N_ACTIONS = 2
# K Q J
N_CARDS = 3


class InformationSet():
    def __init__(self, key):
        self.key = key
        self.regret_sum = np.zeros(N_ACTIONS)
        self.strategy_sum = np.zeros(N_ACTIONS)
        self.strategy = np.array([1/N_ACTIONS] * N_ACTIONS)
        self.reach_prob = 0
        self.reach_prob_sum = 0


    def update_strategy(self):
        #Update strategy on each iteration

        self.strategy_sum += self.reach_prob * self.strategy
        self.strategy = self.get_strategy()
        self.reach_prob_sum += self.reach_prob
        self.reach_prob = 0

    def get_strategy(self):
        # Compute the current iteration strategy

        strategy = self.make_positive(self.regret_sum)
        total = sum(strategy)
        if total > 0:
            strategy = strategy / total
        else:
            strategy = np.repeat(1/N_ACTIONS, N_ACTIONS)

        return strategy

    def get_average_strategy(self):
        # Compute average strategy over all iterations. This is the Nash Equilibrium strategy.

        strategy = self.strategy_sum / self.reach_prob_sum

        # Purify to remove actions that are likely a mistake
        strategy = np.where(strategy < 0.001, 0, strategy)

        # Re-normalizing
        total = sum(strategy)
        strategy /= total

        return strategy

    def make_positive(self, x):
        return np.where(x > 0, x, 0)

    def __str__(self):
        strategy = self.get_average_strategy()
        for s in range(strategy):
            print(self.key, s)


def is_chance_node(history):
    pass

def chance_util(is_map):
    pass

def is_terminal(history):
    pass

def terminal_util(history, card1, card2):
    pass


def card_str(card):
    if card == 0:
        return 'J'
    elif card == 1:
        return 'Q'
    return 'K'

def get_info_set(is_map, card, history):
    # Get information set from map
    key = card_str(card) + " " + history
    information_set = None

    if key not in is_map:
        information_set = InformationSet(key)
        is_map[key] = information_set
        return information_set

    return is_map[key]


def cfr(is_map, history, card_1, card_2, prob_1, prob_2, prob_chance):
    if is_chance_node(history):
        return chance_util(is_map)
    
    if is_terminal(history):
        return terminal_util(history, card_1, card_2)
    
    is_player_1  = len(history) % 2 == 0
    information_set = get_info_set(is_map, card_1 if is_player_1 else card_2, history)

    strategy = information_set.strategy
    information_set.reach_prob += prob_1 if is_player_1 else prob_2

    # Counterfactual utility per action.
    action_utils = np.zeros(N_ACTIONS)

    for i, action in enumerate(['c','b']):
        next_hist  = history + action
        if is_player_1:
            action_utils[i] = -1 * cfr(is_map, next_hist, card_1, card_2, prob_1 * strategy[i], prob_2, prob_chance)
        else:
            action_utils[i] = -1 * cfr(is_map, next_hist, card_1, card_2, prob_1, prob_2 * strategy[i], prob_chance)

    util = sum(action_utils * strategy)
    regrets = action_utils - util

    if is_player_1:
        information_set.regret_sum += prob_chance * prob_2 * regrets
    else:
        information_set.regret_sum += prob_chance * prob_1 * regrets

def main():
    #Information Set Map
    is_map = {}
    n_iterations = 10000
    ev = 0

    for _ in range(n_iterations):
        ev += cfr(is_map)
        for _, u in is_map.items():
            u.update_strategy()

    ev /= n_iterations
